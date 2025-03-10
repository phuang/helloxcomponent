#include "hello/GLCore.h"

#include <cstring>
#include <string>

#include "common/log.h"

namespace hello {
namespace {

void PrintEGLExtensions(EGLDisplay display) {
  const char* extensions = eglQueryString(display, EGL_EXTENSIONS);
  LOGE("%{public}s", extensions);
  size_t extlen = strlen(extensions);
  const char* end = extensions + strlen(extensions);


  while (extensions < end) {
      /* Skip whitespaces, if any */
      if (*extensions == ' ') {
          extensions++;
          continue;
      }

      size_t n = strcspn(extensions, " ");
      std::string extension(extensions, n);
      extensions += n;

      LOGE("    %{public}s", extension.c_str());
  }
}

}  // namespace

GLCore::GLCore() {}

GLCore::~GLCore() {}

void GLCore::InitializeFunctions() {
  eglGetPlatformDisplayEXTFn_ =
      reinterpret_cast<PFNEGLGETPLATFORMDISPLAYEXTPROC>(
          eglGetProcAddress("eglGetPlatformDisplayEXT"));
  FATAL_IF(eglGetPlatformDisplayEXTFn_ == nullptr,
           "eglGetProcAddress(eglGetPlatformDisplayEXT) failed");

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

void GLCore::Init() {
  InitializeFunctions();

  display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  FATAL_IF(display_ == EGL_NO_DISPLAY, "eglGetPlatformDisplayEXT() failed");

  EGLint major, minor;
  if (!eglInitialize(display_, &major, &minor)) {
    FATAL("eglInitialize() failed");
  }
  LOGE("eglInitialize() major=%{public}d minor=%{public}d", major, minor);
  PrintEGLExtensions(display_);

  if (eglBindAPI(EGL_OPENGL_ES_API) == EGL_FALSE) {
    FATAL("Failed to bind OpenGL ES API");
  }

  EGLConfig config;
  const EGLint config_attribs[] = {EGL_SURFACE_TYPE,
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
  EGLint num_configs;
  if (!eglChooseConfig(display_, config_attribs, &config, 1, &num_configs)) {
    FATAL("eglChooseConfig() failed");
  }

  const EGLint context_attribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
  context_ =
      eglCreateContext(display_, config, EGL_NO_CONTEXT, context_attribs);
  FATAL_IF(context_ == EGL_NO_CONTEXT, "eglCreateContext() failed");

  if (!eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, context_)) {
    FATAL("eglMakeCurrent() failed");
  }
}

void GLCore::Destroy() {
  LOGE("GLCore::Destroy()");
  eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
  eglDestroyContext(display_, context_);
  eglTerminate(display_);
}

}  // namespace hello
