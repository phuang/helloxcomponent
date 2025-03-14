//
// Created by huang on 2025-01-22.
//

#include "hello/GLFence.h"

#include "hello/Log.h"
#include "hello/GLCore.h"
#include "hello/NapiManager.h"

namespace hello {

GLFence::GLFence() = default;

GLFence::~GLFence() {
  if (sync_ != EGL_NO_SYNC_KHR) {
    const auto* gl_core = NapiManager::GetInstance()->gl_core();
    EGLDisplay display = gl_core->display();
    gl_core->eglDestroySyncKHR(display, sync_);
    DCHECK_EGL_ERROR();
  }
}

// static
std::shared_ptr<GLFence> GLFence::Create() {
  auto fence = std::make_shared<GLFence>();
  if (!fence->init(EGL_SYNC_NATIVE_FENCE_ANDROID, nullptr)) {
    return nullptr;
  }
  return fence;
}

// static
std::shared_ptr<GLFence> GLFence::CreateFromFenceFd(ScopedFd fenceFd) {
  EGLint attribs[] = {EGL_SYNC_NATIVE_FENCE_FD_ANDROID, fenceFd.release(),
                      EGL_NONE};
  auto fence = std::make_shared<GLFence>();
  if (!fence->init(EGL_SYNC_NATIVE_FENCE_ANDROID, attribs)) {
    return nullptr;
  }
  return fence;
}

bool GLFence::init(EGLint type, const EGLint* attribs) {
  const auto* gl_core = NapiManager::GetInstance()->gl_core();
  EGLDisplay display = gl_core->display();
  sync_ = gl_core->eglCreateSyncKHR(display, type, attribs);
  if (sync_ == EGL_NO_SYNC_KHR) {
    LOGE("eglCreateSyncKHR() faile: 0x%{public}X", eglGetError());
    return false;
  }
  return true;
}

void GLFence::Wait() {
  const auto* gl_core = NapiManager::GetInstance()->gl_core();
  EGLDisplay display = gl_core->display();
  LOGE("EEEE display = %{public}p", display);

  if (gl_core->eglWaitSyncKHR(display, sync_, 0) == EGL_FALSE) {
    LOGE("eglWaitSyncKHR() faile: 0x%{public}X", eglGetError());
  }
}

ScopedFd GLFence::GetFd() {
  const auto* gl_core = NapiManager::GetInstance()->gl_core();
  EGLDisplay display = gl_core->display();
  EGLint fd = gl_core->eglDupNativeFenceFDANDROID(display, sync_);
  if (fd == EGL_NO_NATIVE_FENCE_FD_ANDROID) {
    LOGE("eglDupNativeFenceFDANDROID() faile: 0x%{public}X", eglGetError());
    return {};
  }
  return ScopedFd(fd);
}

}  // namespace hello
