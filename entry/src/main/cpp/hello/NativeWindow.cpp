#include "hello/NativeWindow.h"

#include "hello/Log.h"

namespace hello {

NativeWindow::NativeWindow() = default;

NativeWindow::~NativeWindow() {
  if (image_) {
    OH_NativeImage_Destroy(&image_);
  }
}

// static
std::unique_ptr<NativeWindow> NativeWindow::Create(int32_t width,
                                                   int32_t height,
                                                   int32_t format,
                                                   uint64_t usages) {
  std::unique_ptr<NativeWindow> native_window(new NativeWindow());
  if (!native_window->Initialize(width, height, format, usages)) {
    native_window = nullptr;
  }
  return native_window;
}

bool NativeWindow::Initialize(int32_t width,
                              int32_t height,
                              int32_t format,
                              uint64_t usages) {
  image_ = OH_ConsumerSurface_Create();
  if (!image_) {
    LOGE("OH_ConsumerSurface_Create() failed");
    return false;
  }

  int32_t retval = OH_ConsumerSurface_SetDefaultUsage(image_, usages);
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

  return true;
}

}  // namespace hello
