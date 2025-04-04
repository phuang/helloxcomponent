#ifndef HELLO_NATIVEBUFFER_H
#define HELLO_NATIVEBUFFER_H

#include <stdint.h>

#include <memory>

#include "native_buffer/native_buffer.h"
#include "hello/ScopedFd.h"

namespace hello {

class NativeBuffer : public std::enable_shared_from_this<NativeBuffer> {
 public:
  constexpr static int32_t kDefaultFormat = NATIVEBUFFER_PIXEL_FMT_RGBA_8888;
  constexpr static int32_t kDefaultUsage =
      NATIVEBUFFER_USAGE_CPU_READ | NATIVEBUFFER_USAGE_CPU_WRITE |
      NATIVEBUFFER_USAGE_HW_RENDER | NATIVEBUFFER_USAGE_HW_TEXTURE;

  static std::shared_ptr<NativeBuffer> Create(int32_t width,
                                              int32_t height,
                                              int32_t format = kDefaultFormat,
                                              int32_t usage = kDefaultUsage,
                                              int32_t stride = 0);

  ~NativeBuffer();

  void* Map();
  void Unmap();

  OH_NativeBuffer* buffer() const { return buffer_; }
  int32_t width() const { return config_.width; }
  int32_t height() const { return config_.height; }
  int32_t format() const { return config_.format; }
  int32_t usage() const { return config_.usage; }
  int32_t stride() const { return config_.stride; }

 private:
  NativeBuffer(const NativeBuffer&) = delete;
  NativeBuffer(NativeBuffer&&) = delete;
  NativeBuffer& operator=(const NativeBuffer&) = delete;
  NativeBuffer& operator=(NativeBuffer&&) = delete;

  explicit NativeBuffer(OH_NativeBuffer* buffer);

  OH_NativeBuffer_Config config_ = {};
  OH_NativeBuffer* buffer_ = nullptr;
  ScopedFd fence_fd_;
};

}  // namespace hello

#endif  // HELLO_NATIVEBUFFER_H
