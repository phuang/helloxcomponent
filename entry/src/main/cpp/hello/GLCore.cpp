#include "hello/GLCore.h"

#include "common/log.h"

namespace hello {

GLCore::GLCore() {}

GLCore::~GLCore() {}

void GLCore::Init() {
  display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  FATAL_IF(display_ == EGL_NO_DISPLAY, "eglGetDisplay() failed");

  EGLint major, minor;
  if (!eglInitialize(display_, &major, &minor)) {
    FATAL("eglInitialize() failed");
  }

  if (eglBindAPI(EGL_OPENGL_ES_API) == EGL_FALSE) {
    FATAL("Failed to bind OpenGL ES API");
  }

  EGLConfig config;
  EGLint config_attribs[] = {EGL_SURFACE_TYPE,
                             EGL_WINDOW_BIT,
                             EGL_RED_SIZE,
                             8,
                             EGL_GREEN_SIZE,
                             8,
                             EGL_BLUE_SIZE,
                             8,
                             EGL_ALPHA_SIZE,
                             8,
                             EGL_RENDERABLE_TYPE,
                             EGL_OPENGL_ES3_BIT,
                             EGL_NONE};
  EGLint numConfigs;
  if (!eglChooseConfig(display_, config_attribs, &config, 1, &numConfigs)) {
    FATAL("eglChooseConfig() failed");
  }

  EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
  context_ = eglCreateContext(display_, config, EGL_NO_CONTEXT, contextAttribs);
  FATAL_IF(context_ == EGL_NO_CONTEXT, "eglCreateContext() failed");

  if (!eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, context_)) {
    FATAL("eglMakeCurrent() failed");
  }

  eglCreateSyncKHRFn_ = reinterpret_cast<PFNEGLCREATESYNCKHRPROC>(
      eglGetProcAddress("eglCreateSyncKHR"));
  FATAL_IF(eglCreateSyncKHRFn_ == nullptr,
           "eglGetProcAddress(eglCreateSyncKHR) failed");

  eglDestroySyncKHRFn_ = reinterpret_cast<PFNEGLDESTROYSYNCKHRPROC>(
      eglGetProcAddress("eglDestroySyncKHR"));
  FATAL_IF(eglDestroySyncKHRFn_ == nullptr,
           "eglGetProcAddress(eglDestroySyncKHR) failed");

  eglWaitSyncKHRFn_ = reinterpret_cast<PFNEGLWAITSYNCKHRPROC>(
      eglGetProcAddress("eglWaitSyncKHR"));
  FATAL_IF(eglWaitSyncKHRFn_ == nullptr,
           "eglGetProcAddress(eglWaitSyncKHR) failed");

  eglDupNativeFenceFDANDROIDFn_ =
      reinterpret_cast<PFNEGLDUPNATIVEFENCEFDANDROIDPROC>(
          eglGetProcAddress("eglDupNativeFenceFDANDROID"));
  FATAL_IF(eglDupNativeFenceFDANDROIDFn_ == nullptr,
           "eglGetProcAddress(eglDupNativeFenceFDANDROID) failed");

  eglCreateImageKHRFn_ = reinterpret_cast<PFNEGLCREATEIMAGEKHRPROC>(
      eglGetProcAddress("eglCreateImageKHR"));
  FATAL_IF(eglCreateImageKHRFn_ == nullptr,
           "eglGetProcAddress(eglCreateImageKHR) failed");

  eglDestroyImageKHRFn_ = reinterpret_cast<PFNEGLDESTROYIMAGEKHRPROC>(
      eglGetProcAddress("eglDestroyImageKHR"));
  FATAL_IF(eglDestroyImageKHRFn_ == nullptr,
           "eglGetProcAddress(eglDestroyImageKHR) failed");

  glEGLImageTargetTexture2DOESFn_ =
      reinterpret_cast<PFNGLEGLIMAGETARGETTEXTURE2DOESPROC>(
          eglGetProcAddress("glEGLImageTargetTexture2DOES"));
  FATAL_IF(glEGLImageTargetTexture2DOESFn_ == nullptr,
           "eglGetProcAddress(glEGLImageTargetTexture2DOES) failed");
}

void GLCore::Draw() {
  glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  eglSwapBuffers(display_, surface_);
}

void GLCore::Destroy() {
  eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
  eglDestroyContext(display_, context_);
  eglTerminate(display_);
}

}  // namespace hello
