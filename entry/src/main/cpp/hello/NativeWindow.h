#ifndef HELLO_NATIVEWINDOW_H_
#define HELLO_NATIVEWINDOW_H_

#include <native_buffer/native_buffer.h>
#include <native_image/native_image.h>
#include <native_window/external_window.h>

#include <memory>
#include <optional>

namespace hello {

class NativeWindow {
 public:
  static constexpr int32_t kDefaultFormat = NATIVEBUFFER_PIXEL_FMT_RGBA_8888;
  static constexpr uint64_t kDefaultUsages = NATIVEBUFFER_USAGE_HW_RENDER |
                                             NATIVEBUFFER_USAGE_HW_TEXTURE |
                                             NATIVEBUFFER_USAGE_CPU_WRITE;

  static std::unique_ptr<NativeWindow> Create(
      int32_t width,
      int32_t height,
      int32_t format = kDefaultFormat,
      uint64_t usages = kDefaultUsages);

  ~NativeWindow();

  // Delete copy constructor and copy assignment operator
  NativeWindow(const NativeWindow&) = delete;
  NativeWindow& operator=(const NativeWindow&) = delete;

  // Delete move constructor and move assignment operator
  NativeWindow(NativeWindow&&) = delete;
  NativeWindow& operator=(NativeWindow&&) = delete;

 private:
  NativeWindow();
  bool Initialize(int32_t width,
                  int32_t height,
                  int32_t format,
                  uint64_t usages);

  OH_NativeImage* image_ = nullptr;
  OHNativeWindow* window_ = nullptr;
};

}  // namespace hello

#endif  // HELLO_NATIVEWINDOW_H_
