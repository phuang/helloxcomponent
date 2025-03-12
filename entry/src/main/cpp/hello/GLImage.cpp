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
    DCHECK_EGL_ERROR();
  }
}

GLTexture GLImage::Bind() {
  auto* gl_core = NapiManager::GetInstance()->gl_core();

  GLTexture texture = GLTexture::Create(GL_TEXTURE_EXTERNAL_OES);

  glBindTexture(GL_TEXTURE_EXTERNAL_OES, texture.id());
  gl_core->glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, egl_image_);

  DCHECK_GL_ERROR();

  return texture;
}

}  // namespace hello
