#ifndef HELLO_GL_CORE_H
#define HELLO_GL_CORE_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>

namespace hello {

class GLCore {
 public:
  GLCore();
  ~GLCore();

  void Init();
  void Draw();
  void Destroy();

  EGLDisplay display() const { return display_; }
  EGLContext context() const { return context_; }

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
  EGLDisplay display_ = EGL_NO_DISPLAY;
  EGLSurface surface_ = EGL_NO_SURFACE;
  EGLContext context_ = EGL_NO_CONTEXT;

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
