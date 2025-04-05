#include "hello/Thread.h"

#include <node_api.h>

#include "hello/Log.h"
#include "hello/NapiManager.h"

namespace hello {

Thread::Thread(uv_loop_t* loop) {
  if (loop == nullptr) {
    const auto& env = NapiManager::GetInstance()->env();
    napi_status status = napi_get_uv_event_loop(env, &loop);
    FATAL_IF(status != napi_ok, "napi_get_uv_event_loop() status:=%{public}d",
             status);
  }
  uv_async_init(loop, &uv_async_, [](uv_async_t* handle) {
    auto* self = reinterpret_cast<Thread*>(handle->data);
    self->CallDones();
  });
  uv_async_.data = this;
}

Thread::~Thread() {
  Stop();
}

void Thread::Start() {
  stop_thread_ = false;
  thread_ = std::thread(&Thread::Loop, this);
}

void Thread::Stop() {
  {
    std::unique_lock<std::mutex> lock(mutex_);
    stop_thread_ = true;
  }
  cv_.notify_all();
  if (thread_.joinable()) {
    thread_.join();
  }
}

void Thread::PostTask(const std::function<void()>& task) {
  {
    std::unique_lock<std::mutex> lock(mutex_);
    task_queue_.push(task);
  }
  cv_.notify_one();
}

void Thread::PostTask(const std::function<void()>& task,
                      const std::function<void()>& done) {
  // create napi thread safe function

  // Run task in this thread, and run done in the loop thread.
  PostTask([this, task, done] {
    task();
    {
      std::unique_lock<std::mutex> lock(done_mutex_);
      done_queue_.push(done);
    }
    uv_async_send(&uv_async_);
  });
}

void Thread::Loop() {
  while (true) {
    std::function<void()> task;
    {
      std::unique_lock<std::mutex> lock(mutex_);
      cv_.wait(lock, [this] { return stop_thread_ || !task_queue_.empty(); });

      if (stop_thread_ && task_queue_.empty()) {
        break;
      }

      task = std::move(task_queue_.front());
      task_queue_.pop();
    }
    task();
  }
}

void Thread::CallDones() {
  std::queue<std::function<void()>> done_queue;
  {
    std::unique_lock<std::mutex> lock(done_mutex_);
    done_queue.swap(done_queue_);
  }
  while (!done_queue.empty()) {
    done_queue.front()();
    done_queue.pop();
  }
}

}  // namespace hello
