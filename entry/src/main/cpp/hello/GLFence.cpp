//
// Created by huang on 2025-01-22.
//

#include "hello/GLFence.h"

#include "common/log.h"
#include "hello/GLCore.h"
#include "hello/NapiManager.h"

namespace hello {

GLFence::GLFence() = default;

GLFence::~GLFence() {
  if (sync_ != EGL_NO_SYNC_KHR) {
    const auto* gl_core = NapiManager::GetInstance()->gl_core();
    gl_core->eglDestroySyncKHR(eglGetCurrentDisplay(), sync_);
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
  sync_ = gl_core->eglCreateSyncKHR(eglGetCurrentDisplay(), type, attribs);
  if (sync_ == EGL_NO_SYNC_KHR) {
    LOGE("Failed to eglCreateSyncKHR");
    return false;
  }
  return true;
}

void GLFence::Wait() {
  const auto* gl_core = NapiManager::GetInstance()->gl_core();
  if (gl_core->eglWaitSyncKHR(eglGetCurrentDisplay(), sync_, 0) == EGL_FALSE) {
    LOGE("Failed to eglWaitSyncKHR");
  }
}

ScopedFd GLFence::GetFd() {
  const auto* gl_core = NapiManager::GetInstance()->gl_core();
  EGLint fd =
      gl_core->eglDupNativeFenceFDANDROID(eglGetCurrentDisplay(), sync_);
  if (fd == EGL_NO_NATIVE_FENCE_FD_ANDROID) {
    LOGE("Failed to eglDupNativeFenceFDANDROID");
    return {};
  }
  return ScopedFd(fd);
}

}  // namespace hello
