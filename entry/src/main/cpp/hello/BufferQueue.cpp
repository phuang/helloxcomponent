#include "hello/BufferQueue.h"

#include <optional>

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
  std::unique_lock<std::mutex> lock(mutex_);

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

  avaliable_condition_.notify_all();
}

void BufferQueue::Destroy() {
  std::unique_lock<std::mutex> lock(mutex_);

  if (is_destroyed_) {
    LOGE("BufferQueue::Destroy() BufferQueue is already destroyed");
    return;
  }

  is_destroyed_ = true;
  buffers_.clear();
  avaliable_buffers_.clear();
  produced_buffers_.clear();
  in_present_buffers_.clear();

  // Buffer queue is being destroyed, notify all waiting threads to wake up.
  produced_condition_.notify_all();
  avaliable_condition_.notify_all();
}

std::shared_ptr<NativeBuffer> BufferQueue::RequestBuffer() {
  std::unique_lock<std::mutex> lock(mutex_);

  do {
    if (is_destroyed_) {
      LOGE("BufferQueue::RequestBuffer() BufferQueue is destroyed");
      return nullptr;
    }

    if (!avaliable_buffers_.empty()) {
      auto buffer = std::move(avaliable_buffers_.front());
      avaliable_buffers_.pop_front();
      return buffer;
    }

    if (buffer_count_ < max_buffer_count_) {
      auto buffer = NativeBuffer::Create(width_, height_, format_, usage_);
      CHECK(buffer);
      buffers_.push_back(buffer);
      ++buffer_count_;
      return buffer;
    }

    avaliable_condition_.wait(lock);
  } while (true);
}

void BufferQueue::FlushBuffer(std::shared_ptr<NativeBuffer> buffer) {
  std::unique_lock<std::mutex> lock(mutex_);

  if (is_destroyed_) {
    LOGE("BufferQueue::FlushBuffer() BufferQueue is destroyed");
    return;
  }

  FATAL_IF(!buffer, "Buffer is null");
  produced_buffers_.push_back(std::move(buffer));

  produced_condition_.notify_one();
}

std::shared_ptr<NativeBuffer> BufferQueue::ConsumeBuffer() {
  std::unique_lock<std::mutex> lock(mutex_);

  do {
    if (is_destroyed_) {
      LOGE("BufferQueue::ConsumeBuffer() BufferQueue is destroyed");
      return nullptr;
    }

    if (!produced_buffers_.empty()) {
      auto buffer = std::move(produced_buffers_.front());
      produced_buffers_.pop_front();
      in_present_buffers_[buffer->GetSeqNum()] = buffer;
      return buffer;
    }
    produced_condition_.wait(lock);
  } while (true);
}

void BufferQueue::ReturnBuffer(uint32_t seq_num) {
  std::unique_lock<std::mutex> lock(mutex_);

  if (is_destroyed_) {
    LOGE("BufferQueue::ReturnBuffer() BufferQueue is destroyed");
    return;
  }

  auto it = in_present_buffers_.find(seq_num);
  FATAL_IF(it == in_present_buffers_.end(),
           "BufferQueue::ReturnBuffer() Buffer not found in present buffers");
  auto buffer = std::move(it->second);
  in_present_buffers_.erase(it);

  // size has been changed, free buffers which are not in the same size.
  if (buffer->width() != width_ || buffer->height() != height_) {
    return;
  }

  avaliable_buffers_.push_back(std::move(buffer));
  avaliable_condition_.notify_one();
}

bool BufferQueue::IsDestroyed() const {
  std::unique_lock<std::mutex> lock(mutex_);
  return is_destroyed_;
}

}  // namespace hello
