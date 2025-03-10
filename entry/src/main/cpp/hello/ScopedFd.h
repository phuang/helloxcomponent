//
// Created by huang on 2025-01-23.
//

#ifndef HELLOSURFACECONTROL_SCOPEDFD_H
#define HELLOSURFACECONTROL_SCOPEDFD_H

#include <unistd.h>

namespace hello {

class ScopedFd {
 public:
  explicit ScopedFd(int fd) : fd_(fd) {}
  ScopedFd() : ScopedFd(-1) {}

  ~ScopedFd() { reset(); }

  ScopedFd(const ScopedFd&) = delete;
  ScopedFd& operator=(const ScopedFd&) = delete;

  ScopedFd(ScopedFd&& other) noexcept : ScopedFd(other.fd_) { other.fd_ = -1; }

  ScopedFd& operator=(ScopedFd&& other) noexcept {
    if (this != &other) {
      reset();
      fd_ = other.fd_;
      other.fd_ = -1;
    }
    return *this;
  }

  bool is_valid() const { return fd_ >= 0; }

  void reset(int fd) {
    if (fd < 0) {
      fd = -1;
    }
    if (fd_ != -1) {
      close(fd_);
    }
    fd_ = fd;
  }

  void reset() { reset(-1); }

  int get() const { return fd_; }

  int release() {
    int fd = fd_;
    fd_ = -1;
    return fd;
  }

 private:
  int fd_;
};
}  // namespace hello

#endif  // HELLOSURFACECONTROL_SCOPEDFD_H
