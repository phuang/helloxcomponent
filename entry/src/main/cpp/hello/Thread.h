#ifndef HELLO_THREAD_H
#define HELLO_THREAD_H

#include <uv.h>

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

namespace hello {

class Thread {
 public:
  explicit Thread(uv_loop_t* loop = nullptr);
  ~Thread();

  void Start();
  void Stop();
  void PostTask(const std::function<void()>& task);
  void PostTask(const std::function<void()>& task,
                const std::function<void()>& done);

 private:
  void Loop();
  void CallDones();

  uv_async_t uv_async_;
  std::thread thread_;
  std::mutex mutex_;
  std::condition_variable cv_;
  std::queue<std::function<void()>> task_queue_;
  std::queue<std::function<void()>> done_queue_;
  std::mutex done_mutex_;
  std::atomic<bool> stop_thread_{false};
};

}  // namespace hello

#endif  // HELLO_THREAD_H
