#ifndef HELLO_BUFFER_QUEUE_H_
#define HELLO_BUFFER_QUEUE_H_

#include <deque>
#include <memory>
#include <set>
#include <vector>

#include "hello/NativeBuffer.h"
#include "hello/ScopedFd.h"

namespace hello {

class NativeBuffer;

class BufferQueue : public std::enable_shared_from_this<BufferQueue> {
 public:
  static constexpr int32_t kDefaultMaxBufferCount = 4;
  ~BufferQueue();

  // Create a buffer queue.
  static std::shared_ptr<BufferQueue> Create(
      int32_t width,
      int32_t height,
      int32_t format = NativeBuffer::kDefaultFormat,
      int32_t usage = NativeBuffer::kDefaultUsage,
      int32_t max_buffer_count = kDefaultMaxBufferCount);

  void Resize(int32_t width, int32_t height);

  // Destroy the buffer queue.
  void Destroy();

  // Request a buffer from the queue for production.
  std::shared_ptr<NativeBuffer> RequestBuffer();
  // Flush a buffer from the queue for consumer to consume.
  void FlushBuffer(std::shared_ptr<NativeBuffer> buffer);

  // Consumer a buffer from the queue.
  std::shared_ptr<NativeBuffer> ConsumeBuffer();
  void ReturnBuffer(std::shared_ptr<NativeBuffer> buffer);

 private:
  BufferQueue(const BufferQueue&) = delete;
  BufferQueue& operator=(const BufferQueue&) = delete;
  BufferQueue(BufferQueue&&) = delete;
  BufferQueue& operator=(BufferQueue&&) = delete;

  BufferQueue(int32_t format, int32_t usage, int32_t max_buffer_count);

  const int32_t format_;
  const int32_t usage_;
  const int32_t max_buffer_count_;
  int32_t width_ = 0;
  int32_t height_ = 0;
  size_t buffer_count_ = 0;
  std::vector<std::shared_ptr<NativeBuffer>> buffers_;
  std::deque<std::shared_ptr<NativeBuffer>> avaliable_buffers_;
  std::deque<std::shared_ptr<NativeBuffer>> prodcued_buffers_;
  std::set<std::shared_ptr<NativeBuffer>> in_present_buffers_;
};

}  // namespace hello

#endif  // HELLO_BUFFER_QUEUE_H_
