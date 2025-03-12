#ifndef HELLO_GLTEXTURE_H_
#define HELLO_GLTEXTURE_H_

#include <GLES3/gl3.h>

namespace hello {

class GLTexture {
 public:
  GLTexture();
  GLTexture(GLenum target, GLuint id);
  ~GLTexture();

  static GLTexture Create(GLenum target = GL_TEXTURE_2D);

  // Move constructor
  GLTexture(GLTexture&& other);

  // Move assignment operator
  GLTexture& operator=(GLTexture&& other);

  GLTexture(const GLTexture& other) = delete;
  GLTexture& operator=(const GLTexture& other) = delete;

  GLenum target() const { return target_; }
  GLuint id() const { return id_; }

  void reset();

 private:
  GLenum target_ = GL_NONE;
  GLuint id_ = 0;
};

}  // namespace hello

#endif
