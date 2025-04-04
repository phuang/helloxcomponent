#include "hello/BufferQueue.h"

#include "hello/Log.h"

namespace hello {

BufferQueue::BufferQueue(int32_t format,
                         int32_t usage,
                         int32_t max_buffer_count)
    : format_(format), usage_(usage), max_buffer_count_(max_buffer_count) {}

BufferQueue::~BufferQueue() = default;

// static
std::shared_ptr<BufferQueue> BufferQueue::Create(int32_t width,
                                                 int32_t height,
                                                 int32_t format,
                                                 int32_t usage,
                                                 int32_t max_buffer_count) {
  return std::shared_ptr<BufferQueue>(
      new BufferQueue(format, usage, max_buffer_count));
}

void BufferQueue::Resize(int32_t width, int32_t height) {
  if (is_destroyed_) {
    LOGE("BufferQueue::Resize() BufferQueue is destroyed");
    return;
  }

  if (width == width_ && height == height_) {
    return;
  }

  width_ = width;
  height_ = height;

  buffer_count_ = 0;
  buffers_.clear();
  avaliable_buffers_.clear();
}

void BufferQueue::Destroy() {
  if (is_destroyed_) {
    LOGE("BufferQueue::Destroy() BufferQueue is already destroyed");
    return;
  }

  is_destroyed_ = true;
  buffers_.clear();
  avaliable_buffers_.clear();
  prodcued_buffers_.clear();
  in_present_buffers_.clear();
}

std::shared_ptr<NativeBuffer> BufferQueue::RequestBuffer() {
  if (is_destroyed_) {
    LOGE("BufferQueue::RequestBuffer() BufferQueue is destroyed");
    return {};
  }

  std::shared_ptr<NativeBuffer> buffer;
  if (!avaliable_buffers_.empty()) {
    buffer = std::move(avaliable_buffers_.front());
    avaliable_buffers_.pop_front();
    return buffer;
  }

  if (buffer_count_ < max_buffer_count_) {
    buffer = NativeBuffer::Create(width_, height_, format_, usage_);
    buffers_.push_back(buffer);
    ++buffer_count_;
    return buffer;
  }

  LOGE("BufferQueue::RequestBuffer() max buffer count reached");
  return nullptr;  // No available buffers and max buffer count reached.
}

void BufferQueue::FlushBuffer(std::shared_ptr<NativeBuffer> buffer) {
  if (is_destroyed_) {
    LOGE("BufferQueue::FlushBuffer() BufferQueue is destroyed");
    return;
  }

  FATAL_IF(!buffer, "Buffer is null");
  prodcued_buffers_.push_back(std::move(buffer));
}

std::shared_ptr<NativeBuffer> BufferQueue::ConsumeBuffer() {
  if (is_destroyed_) {
    LOGE("BufferQueue::ConsumeBuffer() BufferQueue is destroyed");
    return{};
  }

  if (prodcued_buffers_.empty()) {
    return nullptr;  // No buffers available for consumption.
  }

  auto buffer = std::move(prodcued_buffers_.front());
  prodcued_buffers_.pop_front();
  return buffer;
}

void BufferQueue::ReturnBuffer(std::shared_ptr<NativeBuffer> buffer) {
  if (is_destroyed_) {
    LOGE("BufferQueue::ReturnBuffer() BufferQueue is destroyed");
    return;
  }

  FATAL_IF(!buffer, "Buffer is null");
  // size has been changed, free buffers which are not in the same size.
  if (buffer->width() != width_ || buffer->height() != height_) {
    return;
  }
  avaliable_buffers_.push_back(std::move(buffer));
}

}  // namespace hello
