#ifndef HELLO_GLIMAGE_H_
#define HELLO_GLIMAGE_H_

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>
#include <native_image/native_image.h>

#include <memory>

namespace hello {

class GLTexture {
 public:
  GLTexture(GLenum target, GLuint texture)
      : target_(target), texture_(texture) {}

  ~GLTexture() {
    reset();
  }

  // Move constructor
  GLTexture(GLTexture&& other) noexcept
      : target_(other.target_), texture_(other.texture_) {
      other.target_ = GL_NONE;
      other.texture_ = 0;
  }

  // Move assignment operator
  GLTexture& operator=(GLTexture&& other) noexcept {
    if (this != &other) {
      reset();
      target_ = other.target_;
      texture_ = other.texture_;
      other.target_ = GL_NONE;
      other.texture_ = 0;
    }
    return *this;
  }

  GLTexture(const GLTexture& other) = delete;
  GLTexture& operator=(const GLTexture& other)  = delete;

  GLenum target() const { return target_; }
  GLuint texture() const { return texture_; }

  void reset() {
    if (texture_) {
      glDeleteTextures(1, &texture_);
      texture_ = 0;
    }
  }

 private:
  GLenum target_ = GL_NONE;
  GLuint texture_ = 0;
};

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
