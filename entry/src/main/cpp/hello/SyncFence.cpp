#include "hello/SyncFence.h"

#include <errno.h>
#include <poll.h>

#include <memory>

namespace hello {

SyncFence::SyncFence(ScopedFd fd) : fd_(std::move(fd)) {}

SyncFence::~SyncFence() = default;

int32_t SyncFence::Wait(int32_t timeout) {
  int retval = -1;
  if (!fd_.is_valid()) {
    return retval;
  }

  struct pollfd pollfds = {0};
  pollfds.fd = fd_.get();
  pollfds.events = POLLIN;

  do {
    retval = poll(&pollfds, 1, timeout);
  } while (retval == -1 && (errno == EINTR || errno == EAGAIN));

  if (retval == 0) {
    retval = -1;
    errno = ETIME;
  } else if (retval > 0) {
    retval = 0;
    if (pollfds.revents & (POLLERR | POLLNVAL)) {
      retval = -1;
      errno = EINVAL;
    }
  }

  return retval < 0 ? -errno : 0;
}

}  // namespace hello
