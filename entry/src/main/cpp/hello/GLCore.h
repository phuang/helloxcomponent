#ifndef HELLO_GL_CORE_H
#define HELLO_GL_CORE_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>

namespace hello {

#define CHECK_EGL_ERROR()                                                    \
  {                                                                          \
    EGLint error = eglGetError();                                            \
    FATAL_IF(error != EGL_SUCCESS,                                           \
             "%{public}s:%{public}d: Got EGL error: 0x%{public}x", __FILE__, \
             __LINE__, error);                                               \
  }

#define CHECK_GL_ERROR()                                                    \
  {                                                                         \
    GLenum error = glGetError();                                            \
    FATAL_IF(error != GL_NO_ERROR,                                          \
             "%{public}s:%{public}d: Got GL error: 0x%{public}x", __FILE__, \
             __LINE__, error);                                              \
  }

#if defined(NDEBUG)
#define DCHECK_EGL_ERROR()
#define DCHECK_GL_ERROR()
#else
#define DCHECK_EGL_ERROR() CHECK_EGL_ERROR()
#define DCHECK_GL_ERROR() CHECK_GL_ERROR()
#endif

class GLCore {
 public:
  static GLuint CreateShader(GLenum type, const char* source);
  static GLuint CreateProgram(const char* vs, const char* fs);

  GLCore();
  ~GLCore();

  void Init();
  void Destroy();

  EGLDisplay display() const { return display_; }
  EGLConfig config() const { return config_; }
  EGLContext context() const { return context_; }

  EGLDisplay eglGetPlatformDisplayEXT(EGLenum platform,
                                      void* native_display,
                                      const EGLint* attrib_list) {
    return eglGetPlatformDisplayEXTFn_(platform, native_display, attrib_list);
  }

  EGLSyncKHR eglCreateSyncKHR(EGLDisplay dpy,
                              EGLenum type,
                              const EGLint* attrib_list) const {
    return eglCreateSyncKHRFn_(dpy, type, attrib_list);
  }

  EGLBoolean eglDestroySyncKHR(EGLDisplay dpy, EGLSyncKHR sync) const {
    return eglDestroySyncKHRFn_(dpy, sync);
  }

  EGLint eglWaitSyncKHR(EGLDisplay dpy, EGLSyncKHR sync, EGLint flags) const {
    return eglWaitSyncKHRFn_(dpy, sync, flags);
  }

  EGLint eglDupNativeFenceFDANDROID(EGLDisplay dpy, EGLSyncKHR sync) const {
    return eglDupNativeFenceFDANDROIDFn_(dpy, sync);
  }

  EGLImageKHR eglCreateImageKHR(EGLDisplay dpy,
                                EGLContext ctx,
                                EGLenum target,
                                EGLClientBuffer buffer,
                                const EGLint* attrib_list) const {
    return eglCreateImageKHRFn_(dpy, ctx, target, buffer, attrib_list);
  }

  EGLBoolean eglDestroyImageKHR(EGLDisplay dpy, EGLImageKHR image) const {
    return eglDestroyImageKHRFn_(dpy, image);
  }

  void glEGLImageTargetTexture2DOES(GLenum target, GLeglImageOES image) const {
    glEGLImageTargetTexture2DOESFn_(target, image);
  }

 private:
  void InitializeFunctions();

  EGLDisplay display_ = EGL_NO_DISPLAY;
  EGLConfig config_ = EGL_NO_CONFIG_KHR;
  EGLSurface surface_ = EGL_NO_SURFACE;
  EGLContext context_ = EGL_NO_CONTEXT;

  // EGL and GL functions
  PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXTFn_ = nullptr;

  PFNEGLCREATESYNCKHRPROC eglCreateSyncKHRFn_ = nullptr;
  PFNEGLDESTROYSYNCKHRPROC eglDestroySyncKHRFn_ = nullptr;
  PFNEGLWAITSYNCKHRPROC eglWaitSyncKHRFn_ = nullptr;
  PFNEGLDUPNATIVEFENCEFDANDROIDPROC eglDupNativeFenceFDANDROIDFn_ = nullptr;

  PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHRFn_ = nullptr;
  PFNEGLDESTROYIMAGEKHRPROC eglDestroyImageKHRFn_ = nullptr;

  PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOESFn_ = nullptr;
};

}  // namespace hello

#endif  // HELLO_GL_CORE_H
