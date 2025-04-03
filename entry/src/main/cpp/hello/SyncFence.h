#ifndef HELLO_SYNCFENCE_H_
#define HELLO_SYNCFENCE_H_

#include <cstdint>

#include "hello/ScopedFd.h"

namespace hello {

class SyncFence {
 public:
  explicit SyncFence(ScopedFd fd);
  ~SyncFence();

  int32_t Wait(int32_t timeout);

 private:
  SyncFence(const SyncFence&) = delete;
  SyncFence& operator=(const SyncFence&) = delete;
  SyncFence(SyncFence&&) = delete;
  SyncFence& operator=(SyncFence&&) = delete;

  ScopedFd fd_;
};

}  // namespace hello

#endif  // HELLO_SYNCFENCE_H_
