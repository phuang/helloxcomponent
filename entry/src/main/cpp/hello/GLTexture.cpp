#include "hello/GLTexture.h"

#include <GLES3/gl3.h>

namespace hello {

// static
GLTexture GLTexture::Create(GLenum target) {
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(target, texture);
  glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return GLTexture(target, texture);
}

GLTexture::GLTexture() = default;

GLTexture::GLTexture(GLenum target, GLuint id)
    : target_(target), id_(id) {}

GLTexture::~GLTexture() {
  reset();
}

GLTexture::GLTexture(GLTexture&& other)
    : target_(other.target_), id_(other.id_) {
  other.target_ = GL_NONE;
  other.id_ = 0;
}

GLTexture& GLTexture::operator=(GLTexture&& other) {
  if (this != &other) {
    reset();
    target_ = other.target_;
    id_ = other.id_;
    other.target_ = GL_NONE;
    other.id_ = 0;
  }
  return *this;
}

void GLTexture::reset() {
  if (id_) {
    glDeleteTextures(1, &id_);
    id_ = 0;
  }
}

}  // namespace hello
