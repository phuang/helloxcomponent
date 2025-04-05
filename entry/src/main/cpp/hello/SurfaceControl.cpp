#include "hello/SurfaceControl.h"

#include "native_window/external_window.h"

#include "hello/BufferQueue.h"
#include "hello/Constants.h"
#include "hello/GLFence.h"
#include "hello/Log.h"
#include "hello/NativeBuffer.h"
#include "hello/NativeWindow.h"
#include "hello/Renderer.h"
#include "hello/SyncFence.h"
#include "hello/Thread.h"

namespace hello {

// static
std::unique_ptr<SurfaceControl> SurfaceControl::Create(const char* name,
                                                       NativeWindow* parent,
                                                       bool is_software,
                                                       Renderer* renderer) {
  LOGE("EEEE SurfaceControl::Create() parent=%{public}p name=%{public}s",
       parent, name);
  if (auto* surface =
          OH_SurfaceControl_FromNativeWindow(parent->window(), name)) {
    return std::unique_ptr<SurfaceControl>(new SurfaceControl(
        surface, parent->width(), parent->height(), is_software, renderer));
  }
  return {};
}

// static
std::unique_ptr<SurfaceControl> SurfaceControl::Create(const char* name,
                                                       int32_t width,
                                                       int32_t height,
                                                       bool is_software,
                                                       Renderer* renderer) {
  LOGE("EEEE SurfaceControl::Create() name=%{public}s", name);
  if (auto* surface = OH_SurfaceControl_Create(name)) {
    return std::unique_ptr<SurfaceControl>(
        new SurfaceControl(surface, width, height, is_software, renderer));
  }
  return {};
}

SurfaceControl::SurfaceControl(OH_SurfaceControl* surface,
                               int32_t width,
                               int32_t height,
                               bool is_software,
                               Renderer* renderer)
    : surface_(surface),
      width_(width),
      height_(height),
      is_software_(is_software),
      renderer_(renderer) {
  dirty_bits_.set();
  if (!renderer_) {
    return;
  }
  buffer_queue_ = BufferQueue::Create(width, height);

  if (is_software_) {
    // If is surface with renderer_ for rendering, the xcomponent is rastered
    // with CPU on a separated thread.
    renderer_thread_ = std::make_unique<Thread>();
    renderer_thread_->Start();
  }
}

SurfaceControl::~SurfaceControl() {
  if (surface_) {
    OH_SurfaceControl_Release(surface_);
  }
  if (buffer_queue_) {
    buffer_queue_->Destroy();
    buffer_queue_.reset();
  }
  if (renderer_thread_) {
    renderer_thread_->Stop();
    renderer_thread_.reset();
  }
}

void SurfaceControl::SetPosition(int32_t x, int32_t y) {
  if (x_ != x || y_ != y) {
    x_ = x;
    y_ = y;
    dirty_bits_.set(DirtyBit::kPosition);
  }
}

void SurfaceControl::SetRotation(float degree) {
  if (rotation_ != degree) {
    rotation_ = degree;
    dirty_bits_.set(DirtyBit::kRotation);
  }
}

bool SurfaceControl::Update(OH_SurfaceTransaction* transaction,
                            uint64_t timestamp,
                            uint64_t target_timestamp) {
  if (renderer_) {
    if (is_software_) {
      renderer_thread_->PostTask([this] { this->SoftwareDrawFrame(); });
    } else {
      HardwareDrawFrame();
    }
  }

  bool need_update = dirty_bits_.any();

  if (dirty_bits_.test(DirtyBit::kPosition)) {
    OH_SurfaceTransaction_SetPosition(transaction, surface_, x_, y_);
  }

  if (dirty_bits_.test(DirtyBit::kSize)) {
    OH_Rect rect = {0, 0, width_, height_};
    OH_SurfaceTransaction_SetCrop(transaction, surface_, &rect);
  }

  if (dirty_bits_.test(DirtyBit::kRotation)) {
    OH_SurfaceTransaction_SetRotation(transaction, surface_, 0.0f, 0.0f,
                                      rotation_);
  }

  dirty_bits_.reset();

  auto buffer = buffer_queue_->ConsumeBuffer(/*wait=*/false);
  LOGE("EEEE SurfaceControl::Update() buffer=%{public}p", buffer.get());
  if (buffer) {
    {
      std::lock_guard<std::mutex> lock(mutex_);
      CHECK(in_flight_seqs_.count(buffer->GetSeqNum()) == 0);
      in_flight_seqs_.insert(buffer->GetSeqNum());
    }

    need_update = true;
    using CallbackFunction = std::function<void(int)>;
    auto release_callback = std::make_unique<CallbackFunction>(
        [this, buffer](int release_fence_fd) {
          LOGE("EEEE OnBufferRelease() release_fence_fd=%{public}d",
               release_fence_fd);
          buffer->SetFenceFd(release_fence_fd);
          OnBufferRelease(std::move(buffer));
        });

    OH_SurfaceTransaction_SetBuffer(
        transaction, surface_, buffer->buffer(),
        buffer->TakeFenceFd().release(),
        /*context=*/release_callback.release(),
        [](void* context, int release_fence_fd) {
          std::unique_ptr<CallbackFunction> callback(
              reinterpret_cast<CallbackFunction*>(context));
          (*callback)(release_fence_fd);
        });
  }

  return need_update;
}

void SurfaceControl::SoftwareDrawFrame() {
  LOGE("EEEE SurfaceControl::SoftwareDrawFrame()");
  CHECK(renderer_);
  CHECK(buffer_queue_);

  if (auto buffer = buffer_queue_->RequestBuffer()) {
    auto fence_fd = buffer->TakeFenceFd();
    // SyncFence sync_fence(std::move(fence_fd));
    // sync_fence.Wait();
    void* addr = buffer->Map();
    renderer_->RenderPixels(addr, buffer->width(), buffer->height(),
                            buffer->stride(), 0);
    buffer->Unmap();
    buffer_queue_->FlushBuffer(std::move(buffer));
  }
}

void SurfaceControl::HardwareDrawFrame() {
  CHECK(renderer_);
  CHECK(buffer_queue_);

  auto buffer = buffer_queue_->RequestBuffer(/*wait=*/false);
  if (!buffer) {
    LOGE("EEEE RequestBuffer() failed");
    return;
  }

  if (auto fd = buffer->TakeFenceFd()) {
    if (auto fence = GLFence::CreateFromFenceFd(std::move(fd))) {
      fence->Wait();
    }

    OHNativeWindowBuffer* window_buffer =
        OH_NativeWindow_CreateNativeWindowBufferFromNativeBuffer(
            buffer->buffer());
    FATAL_IF(
        window_buffer == nullptr,
        "OH_NativeWindow_CreateNativeWindowBufferFromNativeBuffer() failed");

    GLImage gl_image;
    if (!gl_image.Initialize(window_buffer)) {
      FATAL("GLImage::Initialize() failed!");
    }
    GLTexture texture = gl_image.Bind();
    renderer_->RenderTexture(texture.target(), texture.id(), width_, height_,
                             0);

    OH_NativeWindow_DestroyNativeWindowBuffer(window_buffer);

    auto fence = GLFence::Create();
    buffer->SetFenceFd(fence->GetFd().release());
  }

  buffer_queue_->FlushBuffer(std::move(buffer));
}

void SurfaceControl::OnBufferRelease(std::shared_ptr<NativeBuffer> buffer) {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    CHECK(in_flight_seqs_.count(buffer->GetSeqNum()) == 1);
    in_flight_seqs_.erase(buffer->GetSeqNum());
  }
  buffer_queue_->ReturnBuffer(std::move(buffer));
}

}  // namespace hello
