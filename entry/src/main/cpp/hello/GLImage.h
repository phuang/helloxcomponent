#ifndef HELLO_GLIMAGE_H_
#define HELLO_GLIMAGE_H_

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>
#include <native_image/native_image.h>

#include <memory>

#include "hello/GLTexture.h"

namespace hello {

class GLImage {
 public:
  GLImage();
  ~GLImage();

  bool Initialize(OHNativeWindowBuffer* window_buffer);
  void Destroy();

  GLTexture Bind();

  EGLImageKHR egl_image() const { return egl_image_; }

 private:
  EGLImageKHR egl_image_ = EGL_NO_IMAGE_KHR;
};

}  // namespace hello
#endif  // HELLO_GLIMAGE_H_
