#include "hello/GLImage.h"

#include "hello/GLCore.h"
#include "hello/Log.h"
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
  if (egl_image_ == EGL_NO_IMAGE_KHR) {
    LOGE("eglCreateImageKHR() faile: 0x%{public}X", eglGetError());
    return false;
  }

  return true;
}

void GLImage::Destroy() {
  if (egl_image_ != EGL_NO_IMAGE_KHR) {
    const auto* gl_core = NapiManager::GetInstance()->gl_core();
    EGLDisplay display = gl_core->display();
    gl_core->eglDestroyImageKHR(display, egl_image_);
    egl_image_ = EGL_NO_IMAGE_KHR;
    CHECK_EGL_ERROR();
  }
}

GLTexture GLImage::Bind() {
  const GLenum kTarget = GL_TEXTURE_EXTERNAL_OES;
  auto* gl_core = NapiManager::GetInstance()->gl_core();
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(kTarget, texture);

  // Set texture parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  gl_core->glEGLImageTargetTexture2DOES(kTarget, egl_image_);

  CHECK_GL_ERROR();

  return GLTexture(kTarget, texture);
}

}  // namespace hello
