#ifndef HELLO_NATIVEWINDOW_H_
#define HELLO_NATIVEWINDOW_H_

#include <native_buffer/native_buffer.h>
#include <native_image/native_image.h>
#include <native_window/external_window.h>

#include <memory>
#include <optional>
#include <deque>

#include "hello/GLTexture.h"
#include "hello/ScopedFd.h"

namespace hello {

class NativeWindow {
 public:
  static constexpr int32_t kDefaultFormat = NATIVEBUFFER_PIXEL_FMT_RGBA_8888;
  static constexpr uint64_t kDefaultUsages = NATIVEBUFFER_USAGE_HW_RENDER |
                                             NATIVEBUFFER_USAGE_HW_TEXTURE |
                                             NATIVEBUFFER_USAGE_CPU_READ |
                                             NATIVEBUFFER_USAGE_CPU_WRITE |
                                             NATIVEBUFFER_USAGE_ALIGNMENT_512 |
                                             NATIVEBUFFER_USAGE_MEM_DMA |
                                             0;

  static std::unique_ptr<NativeWindow> Create(int32_t width,
                                              int32_t height,
                                              int32_t format = kDefaultFormat,
                                              uint64_t usages = kDefaultUsages);

  static std::unique_ptr<NativeWindow> CreateFromNativeWindow(
      OHNativeWindow* window);

  ~NativeWindow();

  // Delete copy constructor and copy assignment operator
  NativeWindow(const NativeWindow&) = delete;
  NativeWindow& operator=(const NativeWindow&) = delete;

  // Delete move constructor and move assignment operator
  NativeWindow(NativeWindow&&) = delete;
  NativeWindow& operator=(NativeWindow&&) = delete;

  // Create GLTexture for comsuming image content.
  GLTexture BindTexture();
  void UpdateSurfaceImage();

  bool RequestBuffer(int32_t* width,
                     int32_t* height,
                     int32_t* stride,
                     ScopedFd* fence_fd,
                     void** addr);
  void FlushBuffer();
  OHNativeWindow* window() const { return window_; }

 private:
  NativeWindow();
  explicit NativeWindow(OHNativeWindow* window);

  bool Initialize(int32_t width,
                  int32_t height,
                  int32_t format,
                  uint64_t usages);

  OH_NativeImage* image_ = nullptr;
  OHNativeWindow* window_ = nullptr;
  std::deque<OHNativeWindowBuffer*> window_buffers_;
  std::deque<OH_NativeBuffer*> buffers_;
};

}  // namespace hello

#endif  // HELLO_NATIVEWINDOW_H_
