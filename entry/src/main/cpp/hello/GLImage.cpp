#include "hello/GLImage.h"

#include "common/log.h"
#include "hello/GLCore.h"
#include "hello/NapiManager.h"

namespace hello {

GLImage::GLImage() = default;

GLImage::~GLImage() {
  Destroy();
}

bool GLImage::Initialize(OHNativeWindowBuffer* window_buffer) {
  const auto* gl_core = NapiManager::GetInstance()->gl_core();
  EGLDisplay display = gl_core->display();

  EGLint attrs[] = {
      EGL_IMAGE_PRESERVED,
      EGL_TRUE,
      EGL_NONE,
  };
  egl_image_ = gl_core->eglCreateImageKHR(
      display, EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_OHOS, window_buffer, attrs);
  FATAL_IF(egl_image_ == EGL_NO_IMAGE_KHR, "eglCreateImageKHR() failed!");

  return true;
}

void GLImage::Destroy() {
  if (egl_image_ != EGL_NO_IMAGE_KHR) {
    const auto* gl_core = NapiManager::GetInstance()->gl_core();
    EGLDisplay display = gl_core->display();
    gl_core->eglDestroyImageKHR(display, egl_image_);
    egl_image_ = EGL_NO_IMAGE_KHR;
  }
}

GLTexture GLImage::Bind() {
  auto* gl_core = NapiManager::GetInstance()->gl_core();
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_EXTERNAL_OES, texture);
  glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  gl_core->glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, egl_image_);

  return GLTexture(GL_TEXTURE_EXTERNAL_OES, texture);
}

}  // namespace hello
