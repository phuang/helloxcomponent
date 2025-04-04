#include "hello/NativeBuffer.h"

#include "hello/Log.h"

namespace hello {

// static
std::shared_ptr<NativeBuffer> NativeBuffer::Create(int32_t width,
                                                   int32_t height,
                                                   int32_t format,
                                                   int32_t usage,
                                                   int32_t stride) {
  OH_NativeBuffer_Config config = {
      .width = width,
      .height = height,
      .format = format,
      .usage = usage,
      .stride = stride,
  };
  if (auto* buffer = OH_NativeBuffer_Alloc(&config)) {
    return std::shared_ptr<NativeBuffer>(new NativeBuffer(buffer));
  }
  return {};
}

NativeBuffer::NativeBuffer(OH_NativeBuffer* buffer) : buffer_(buffer) {
  if (buffer_) {
    OH_NativeBuffer_GetConfig(buffer_, &config_);
  }
}

NativeBuffer::~NativeBuffer() {
  if (buffer_) {
    OH_NativeBuffer_Reference(buffer_);
  }
}

void* NativeBuffer::Map() {
  if (buffer_) {
    void* addr = nullptr;
    int32_t retval = OH_NativeBuffer_Map(buffer_, &addr);
    FATAL_IF(retval != 0, "Map failed, ret:%{public}d", retval);
    return addr;
  }
  return nullptr;
}

void NativeBuffer::Unmap() {
  if (buffer_) {
    int32_t retval = OH_NativeBuffer_Unmap(buffer_);
    FATAL_IF(retval != 0, "Unmap failed, ret:%{public}d", retval);
  }
}
}  // namespace hello
