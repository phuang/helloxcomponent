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
#define GETPROC(name, TYPE)                                     \
  name##Fn_ = reinterpret_cast<TYPE>(eglGetProcAddress(#name)); \
  FATAL_IF(name##Fn_ == nullptr, "eglGetProcAddress(" #name ") failed");

  GETPROC(eglGetPlatformDisplayEXT, PFNEGLGETPLATFORMDISPLAYEXTPROC)
  GETPROC(eglCreateSyncKHR, PFNEGLCREATESYNCKHRPROC)
  GETPROC(eglDestroySyncKHR, PFNEGLDESTROYSYNCKHRPROC)
  GETPROC(eglWaitSyncKHR, PFNEGLWAITSYNCKHRPROC)
  GETPROC(eglDupNativeFenceFDANDROID, PFNEGLDUPNATIVEFENCEFDANDROIDPROC)
  GETPROC(eglCreateImageKHR, PFNEGLCREATEIMAGEKHRPROC)
  GETPROC(eglDestroyImageKHR, PFNEGLDESTROYIMAGEKHRPROC)
  GETPROC(glEGLImageTargetTexture2DOES, PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)
#undef GETPROC
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
  // PrintEGLExtensions(display_);

  if (eglBindAPI(EGL_OPENGL_ES_API) == EGL_FALSE) {
    FATAL("Failed to bind OpenGL ES API");
  }

  // clang-format off
  const EGLint config_attribs[] = {
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_ALPHA_SIZE, 8,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
    EGL_NONE };
  // clang-format on

  EGLint num_configs;
  if (!eglChooseConfig(display_, config_attribs, &config_, 1, &num_configs)) {
    FATAL("eglChooseConfig() failed");
  }

  const EGLint context_attribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
  context_ =
      eglCreateContext(display_, config_, EGL_NO_CONTEXT, context_attribs);
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
