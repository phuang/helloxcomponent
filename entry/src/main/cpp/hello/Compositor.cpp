#include "hello/Compositor.h"

#include "hello/BitmapRenderer.h"
#include "hello/Constants.h"
#include "hello/Log.h"
#include "hello/NativeWindow.h"
#include "hello/TextureRenderer.h"

namespace hello {

Compositor::Compositor() {
  LOGE("Compositor::Compositor()");
  texture_renderer_ = std::make_unique<TextureRenderer>();
  bitmap_renderer_ = std::make_unique<BitmapRenderer>(kPictureRiverUri);

  texture_native_window_ =
      NativeWindow::Create(kEGLSurfaceNodeSize, kEGLSurfaceNodeSize);
  bitmap_native_window_ =
      NativeWindow::Create(kBitmapNodeSize, kBitmapNodeSize);

  // OHNativeWindowBuffer* window_buffer;
  // int fence_fd;
  // retval = OH_NativeWindow_NativeWindowRequestBuffer(window, &window_buffer,
  //                                                    &fence_fd);
  // FATAL_IF(retval != 0,
  //          "OH_NativeWindow_NativeWindowRequestBuffer() failed "
  //          "retval=%{public}d",
  //          retval);
}

Compositor::~Compositor() {}

void Compositor::RenderFrame(int32_t width,
                             int32_t height,
                             uint64_t timestamp) {
  // Texture renderer first
  texture_native_window_.get();

  // BitmapRenderer second
}
}  // namespace hello
