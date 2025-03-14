#include "hello/NativeWindow.h"

#include <GLES2/gl2ext.h>

#include "hello/GLCore.h"
#include "hello/GLFence.h"
#include "hello/Log.h"

namespace hello {

// static
std::unique_ptr<NativeWindow> NativeWindow::Create(int32_t width,
                                                   int32_t height,
                                                   int32_t format,
                                                   uint64_t usage) {
  std::unique_ptr<NativeWindow> native_window(new NativeWindow());
  if (!native_window->Initialize(width, height, format, usage)) {
    native_window = nullptr;
  }
  return native_window;
}

// static
std::unique_ptr<NativeWindow> NativeWindow::CreateFromNativeWindow(
    OHNativeWindow* window,
    uint64_t usage) {
  int32_t ret = OH_NativeWindow_NativeWindowHandleOpt(window, SET_USAGE, usage);
  if (ret != 0) {
    LOGE("OH_NativeWindow_NativeWindowHandleOpt(SET_USAGE) ret:%{public}d",
         ret);
    return nullptr;
  }
  std::unique_ptr<NativeWindow> native_window(new NativeWindow(window));
  return native_window;
}

// static
std::unique_ptr<NativeWindow> NativeWindow::CreateFromSurfaceId(
    uint64_t surface_id,
    uint64_t usage) {
  OHNativeWindow* window = nullptr;
  int ret =
      OH_NativeWindow_CreateNativeWindowFromSurfaceId(surface_id, &window);
  if (ret != 0) {
    LOGE("OH_NativeWindow_CreateNativeWindowFromSurfaceId() ret:%{public}d",
         ret);
    return nullptr;
  }
  return CreateFromNativeWindow(window, usage);
}

NativeWindow::NativeWindow() = default;

NativeWindow::NativeWindow(OHNativeWindow* window) : window_(window) {
  int32_t ret = OH_NativeWindow_GetSurfaceId(window_, &surface_id_);
  if (ret != 0) {
    FATAL("OH_NativeWindow_GetSurfaceId() ret:%{public}d", ret);
  }
}

NativeWindow::~NativeWindow() {
  if (image_) {
    OH_NativeImage_Destroy(&image_);
  }
}

bool NativeWindow::Initialize(int32_t width,
                              int32_t height,
                              int32_t format,
                              uint64_t usage) {
  image_ = OH_ConsumerSurface_Create();
  if (!image_) {
    LOGE("OH_ConsumerSurface_Create() failed");
    return false;
  }

  int32_t ret;
  ret = OH_ConsumerSurface_SetDefaultUsage(image_, usage);
  if (ret != 0) {
    LOGE("OH_ConsumerSurface_SetDefaultUsage() ret:%{public}d", ret);
    return false;
  }

  ret = OH_ConsumerSurface_SetDefaultSize(image_, width, height);
  if (ret != 0) {
    LOGE("OH_ConsumerSurface_SetDefaultSize() ret:%{public}d", ret);
    return false;
  }

  window_ = OH_NativeImage_AcquireNativeWindow(image_);
  if (!window_) {
    LOGE("OH_NativeImage_AcquireNativeWindow() failed");
    return false;
  }

  // ret = OH_NativeWindow_NativeWindowHandleOpt(window_, SET_USAGE, usages);
  // if (ret != 0) {
  //   LOGE(
  //       "OH_NativeWindow_NativeWindowHandleOpt(SET_USAGE) failed "
  //       "ret=%{public}d",
  //       ret);
  //   return false;
  // }

  ret = OH_NativeWindow_NativeWindowHandleOpt(window_, SET_FORMAT, format);
  if (ret != 0) {
    LOGE("OH_NativeWindow_NativeWindowHandleOpt(SET_FROMAT) ret:%{public}d",
         ret);
    return false;
  }

  ret = OH_NativeWindow_GetSurfaceId(window_, &surface_id_);
  if (ret != 0) {
    LOGE("OH_NativeWindow_GetSurfaceId() ret:%{public}d", ret);
    return false;
  }

  OH_OnFrameAvailableListener listener = {
      this, [](void* context) {
        reinterpret_cast<NativeWindow*>(context)->OnFrameAvailable();
      }};
  ret = OH_NativeImage_SetOnFrameAvailableListener(image_, listener);
  if (ret != 0) {
    LOGE("OH_NativeImage_SetOnFrameAvailableListener() ret:%{public}d", ret);
    return false;
  }

  return true;
}

GLTexture NativeWindow::BindTexture() {
  GLTexture texture = GLTexture::Create(GL_TEXTURE_EXTERNAL_OES);

  int32_t ret = OH_NativeImage_AttachContext(image_, texture.id());
  if (ret != 0) {
    LOGE("OH_NativeImage_AttachContext() ret:%{public}d", ret);
    texture.reset();
  }

  return texture;
}

std::unique_ptr<GLImage> NativeWindow::AcquireGLImage() {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    if (avaliable_frames_ == 0) {
      return nullptr;
    }
    --avaliable_frames_;
  }

  int fence_fd = -1;
  OHNativeWindowBuffer* window_buffer = nullptr;
  int32_t ret = OH_NativeImage_AcquireNativeWindowBuffer(image_, &window_buffer,
                                                         &fence_fd);
  if (ret != 0) {
    FATAL("OH_NativeImage_AcquireNativeWindowBuffer() ret:%{public}d", ret);
  }

  auto gl_image = std::make_unique<GLImage>();
  if (!gl_image->Initialize(window_buffer)) {
    FATAL("GLImage::Initialize() failed");
  }

  if (auto fence = GLFence::CreateFromFenceFd(ScopedFd(fence_fd))) {
    fence->Wait();
  }

  acquired_window_buffers_.push_back(window_buffer);
  return gl_image;
}

void NativeWindow::ReleaseGLImage(std::unique_ptr<GLImage> gl_image) {
  CHECK(!acquired_window_buffers_.empty());

  auto fence = GLFence::Create();

  int32_t ret = OH_NativeImage_ReleaseNativeWindowBuffer(
      image_, acquired_window_buffers_.front(), fence->GetFd().release());
  if (ret != 0) {
    FATAL("OH_NativeImage_ReleaseNativeWindowBuffer() ret:%{public}d", ret);
  }
  acquired_window_buffers_.pop_front();
}

bool NativeWindow::UpdateSurfaceImage() {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    if (avaliable_frames_ == 0) {
      return false;
    }
    --avaliable_frames_;
  }

  CHECK(image_);

  DCHECK_GL_ERROR();
  int32_t ret = OH_NativeImage_UpdateSurfaceImage(image_);
  if (ret != 0) {
    FATAL("OH_NativeImage_UpdateSurfaceImage() ret:%{public}d", ret);
  }
  return true;
}

bool NativeWindow::RequestBuffer(int32_t* width,
                                 int32_t* height,
                                 int32_t* stride,
                                 ScopedFd* fence_fd,
                                 void** addr) {
  OHNativeWindowBuffer* window_buffer = nullptr;
  OH_NativeBuffer* buffer = nullptr;

  int fd = -1;
  int32_t ret =
      OH_NativeWindow_NativeWindowRequestBuffer(window_, &window_buffer, &fd);
  FATAL_IF(ret != 0,
           "OH_NativeWindow_NativeWindowRequestBuffer() ret:%{public}d", ret);

  fence_fd->reset(fd);

  ret = OH_NativeBuffer_FromNativeWindowBuffer(window_buffer, &buffer);
  FATAL_IF(ret != 0, "OH_NativeBuffer_FromNativeWindowBuffer() ret:%{public}d",
           ret);

  OH_NativeBuffer_Config config;
  OH_NativeBuffer_GetConfig(buffer, &config);

  ret = OH_NativeBuffer_Map(buffer, addr);
  FATAL_IF(ret != 0, "OH_NativeBuffer_Map() ret:%{public}d", ret);
  *width = config.width;
  *height = config.height;
  *stride = config.stride;

  window_buffers_.push_back(window_buffer);
  buffers_.push_back(buffer);

  return true;
}

void NativeWindow::FlushBuffer() {
  FATAL_IF(buffers_.empty(), "There is no pending buffer");
  FATAL_IF(window_buffers_.empty(), "There is no pending buffer");

  OH_NativeBuffer_Unmap(buffers_.front());
  int32_t ret = OH_NativeWindow_NativeWindowFlushBuffer(
      window_, window_buffers_.front(), -1, {});
  FATAL_IF(ret != 0, "OH_NativeWindow_NativeWindowFlushBuffer() ret:%{public}d",
           ret);

  buffers_.pop_front();
  window_buffers_.pop_front();
}

void NativeWindow::OnFrameAvailable() {
  std::lock_guard<std::mutex> lock(mutex_);
  CHECK(image_);
  ++avaliable_frames_;
}

}  // namespace hello
