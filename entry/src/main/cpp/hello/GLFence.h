#ifndef HELLO_GLFENCE_H
#define HELLO_GLFENCE_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <memory>

#include "hello/ScopedFd.h"

namespace hello {

class GLFence : public std::enable_shared_from_this<GLFence> {
 public:
  GLFence();
  ~GLFence();

  static std::shared_ptr<GLFence> Create();
  static std::shared_ptr<GLFence> CreateFromFenceFd(ScopedFd fenceFd);

  void Wait();
  ScopedFd GetFd();

 private:
  GLFence(const GLFence&) = delete;
  GLFence& operator=(const GLFence&) = delete;
  GLFence(GLFence&&) = delete;
  GLFence& operator=(GLFence&&) = delete;

  bool init(EGLint type, const EGLint* attribs);

  EGLSyncKHR sync_ = EGL_NO_SYNC_KHR;
};

}  // namespace hello

#endif  // HELLO_GLFENCE_H
