#include "hello/SurfaceControl.h"

#include "hello/Constants.h"
#include "hello/Log.h"
#include "hello/NativeBuffer.h"
#include "hello/NativeWindow.h"

namespace hello {

// static
std::unique_ptr<SurfaceControl> SurfaceControl::Create(NativeWindow* parent,
                                                       const char* name) {
  LOGE("EEEE SurfaceControl::Create() parent=%{public}p name=%{public}s",
       parent, name);
  if (auto* surface =
          OH_SurfaceControl_FromNativeWindow(parent->window(), name)) {
    return std::unique_ptr<SurfaceControl>(new SurfaceControl(surface));
  }
  return {};
}

// static
std::unique_ptr<SurfaceControl> SurfaceControl::Create(const char* name) {
  LOGE("EEEE SurfaceControl::Create() name=%{public}s", name);
  if (auto* surface = OH_SurfaceControl_Create(name)) {
    return std::unique_ptr<SurfaceControl>(new SurfaceControl(surface));
  }
  return {};
}

SurfaceControl::SurfaceControl(OH_SurfaceControl* surface) : surface_(surface) {
  buffer_ = NativeBuffer::Create(512, 512);

  void* addr = buffer_->Map();
  if (addr) {
    uint8_t* addr8 = static_cast<uint8_t*>(addr);
    for (int i = 0; i < buffer_->height(); i++) {
      for (int j = 0; j < buffer_->width(); j++) {
        addr8[j * 4 + 0] = i;
        addr8[j * 4 + 1] = 0;
        addr8[j * 4 + 2] = 0x00;
        addr8[j * 4 + 3] = 0xFF;
      }
      addr8 += buffer_->stride();
    }
  }
  buffer_->Unmap();

  auto* transaction = OH_SurfaceTransaction_Create();

  OH_SurfaceTransaction_SetBuffer(transaction, surface_, buffer_->buffer(),
                                  /*acquire_fence_fd=*/-1, this,
                                  SurfaceControl::OnBufferReleaseStub);
  OH_Rect crop = {
      .x = 0,
      .y = 0,
      .w = buffer_->width(),
      .h = buffer_->height(),
  };
  OH_SurfaceTransaction_SetCrop(transaction, surface_, &crop);
  OH_SurfaceTransaction_SetVisibility(transaction, surface_,
                                      OH_SURFACE_TRANSACTION_VISIBILITY_SHOW);
  OH_SurfaceTransaction_SetScale(transaction, surface_, 1.0, 1.0);
  OH_SurfaceTransaction_SetPosition(transaction, surface_, 100, 400);
  OH_SurfaceTransaction_Commit(transaction);
  OH_SurfaceTransaction_Delete(transaction);
}

SurfaceControl::~SurfaceControl() {
  if (surface_) {
    OH_SurfaceControl_Release(surface_);
  }
}

void SurfaceControl::OnBufferRelease(int release_fence_fd) {
  LOGE("EEEE Buffer released");
}

// static
void SurfaceControl::OnBufferReleaseStub(void* context, int release_fence_fd) {
  if (context) {
    auto* self = static_cast<SurfaceControl*>(context);
    self->OnBufferRelease(release_fence_fd);
  }
  if (release_fence_fd >= 0) {
    close(release_fence_fd);
  }
}

}  // namespace hello
