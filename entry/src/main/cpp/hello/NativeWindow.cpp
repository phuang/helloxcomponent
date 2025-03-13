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
  int32_t retval =
      OH_NativeWindow_NativeWindowHandleOpt(window, SET_USAGE, usage);
  if (retval != 0) {
    LOGE(
        "OH_NativeWindow_NativeWindowHandleOpt(SET_USAGE) failed "
        "retval=%{public}d",
        retval);
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
  int retval =
      OH_NativeWindow_CreateNativeWindowFromSurfaceId(surface_id, &window);
  if (retval != 0) {
    LOGE(
        "OH_NativeWindow_CreateNativeWindowFromSurfaceId() failed "
        "retval=%{public}d",
        retval);
    return nullptr;
  }
  return CreateFromNativeWindow(window, usage);
}

NativeWindow::NativeWindow() = default;

NativeWindow::NativeWindow(OHNativeWindow* window) : window_(window) {
  int32_t retval = OH_NativeWindow_GetSurfaceId(window_, &surface_id_);
  if (retval != 0) {
    FATAL("OH_NativeWindow_GetSurfaceId() failed retval=%{public}d", retval);
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

  int32_t retval = OH_ConsumerSurface_SetDefaultUsage(image_, usage);
  if (retval != 0) {
    LOGE("OH_ConsumerSurface_SetDefaultUsage() failed retval=%{public}d",
         retval);
    return false;
  }

  retval = OH_ConsumerSurface_SetDefaultSize(image_, width, height);
  if (retval != 0) {
    LOGE("OH_ConsumerSurface_SetDefaultSize() failed retval=%{public}d",
         retval);
    return false;
  }

  window_ = OH_NativeImage_AcquireNativeWindow(image_);
  if (!window_) {
    LOGE("OH_NativeImage_AcquireNativeWindow() failed");
    return false;
  }

  // retval = OH_NativeWindow_NativeWindowHandleOpt(window_, SET_USAGE, usages);
  // if (retval != 0) {
  //   LOGE(
  //       "OH_NativeWindow_NativeWindowHandleOpt(SET_USAGE) failed "
  //       "retval=%{public}d",
  //       retval);
  //   return false;
  // }

  retval = OH_NativeWindow_NativeWindowHandleOpt(window_, SET_FORMAT, format);
  if (retval != 0) {
    LOGE(
        "OH_NativeWindow_NativeWindowHandleOpt(SET_FROMAT) failed "
        "retval=%{public}d",
        retval);
    return false;
  }

  retval = OH_NativeWindow_GetSurfaceId(window_, &surface_id_);
  if (retval != 0) {
    LOGE("OH_NativeWindow_GetSurfaceId() failed retval=%{public}d", retval);
    return false;
  }

  return true;
}

GLTexture NativeWindow::BindTexture() {
  GLTexture texture = GLTexture::Create(GL_TEXTURE_EXTERNAL_OES);

  int32_t retval = OH_NativeImage_AttachContext(image_, texture.id());
  if (retval != 0) {
    LOGE("OH_NativeImage_AttachContext() failed retval=%{public}d", retval);
    texture.reset();
  }

  return texture;
}

std::unique_ptr<GLImage> NativeWindow::AcquireGLImage() {
  int fence_fd = -1;
  OHNativeWindowBuffer* window_buffer = nullptr;
  int32_t retval = OH_NativeImage_AcquireNativeWindowBuffer(
      image_, &window_buffer, &fence_fd);
  if (retval != 0) {
    FATAL("OH_NativeImage_AcquireNativeWindowBuffer() failed retval=%{public}d",
          retval);
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

  int32_t retval = OH_NativeImage_ReleaseNativeWindowBuffer(
      image_, acquired_window_buffers_.front(), fence->GetFd().release());
  if (retval != 0) {
    FATAL("OH_NativeImage_ReleaseNativeWindowBuffer() failed retval=%{public}d",
          retval);
  }
  acquired_window_buffers_.pop_front();
}

void NativeWindow::UpdateSurfaceImage() {
  CHECK(image_);
  DCHECK_GL_ERROR();
  int32_t retval = OH_NativeImage_UpdateSurfaceImage(image_);
  if (retval != 0) {
    FATAL("OH_NativeImage_UpdateSurfaceImage() failed retval=%{public}d",
          retval);
  }
}

bool NativeWindow::RequestBuffer(int32_t* width,
                                 int32_t* height,
                                 int32_t* stride,
                                 ScopedFd* fence_fd,
                                 void** addr) {
  OHNativeWindowBuffer* window_buffer = nullptr;
  OH_NativeBuffer* buffer = nullptr;

  int fd = -1;
  int32_t retval =
      OH_NativeWindow_NativeWindowRequestBuffer(window_, &window_buffer, &fd);
  FATAL_IF(
      retval != 0,
      "OH_NativeWindow_NativeWindowRequestBuffer() failed retval=%{public}d",
      retval);

  fence_fd->reset(fd);

  retval = OH_NativeBuffer_FromNativeWindowBuffer(window_buffer, &buffer);
  FATAL_IF(retval != 0,
           "OH_NativeBuffer_FromNativeWindowBuffer() failed retval=%{public}d",
           retval);

  OH_NativeBuffer_Config config;
  OH_NativeBuffer_GetConfig(buffer, &config);

  retval = OH_NativeBuffer_Map(buffer, addr);
  FATAL_IF(retval != 0, "OH_NativeBuffer_Map() failed retval=%{public}d",
           retval);
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
  int32_t retval = OH_NativeWindow_NativeWindowFlushBuffer(
      window_, window_buffers_.front(), -1, {});
  FATAL_IF(retval != 0,
           "OH_NativeWindow_NativeWindowFlushBuffer() failed "
           "retval=%{public}d",
           retval);
  // LOGE("EEEE FlushBuffer() window_buffer=%{public}p",
  // window_buffers_.front());

  buffers_.pop_front();
  window_buffers_.pop_front();
}

}  // namespace hello
