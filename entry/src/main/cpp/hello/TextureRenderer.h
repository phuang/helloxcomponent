#ifndef HELLO_TEXTURE_RENDERER_H
#define HELLO_TEXTURE_RENDERER_H

#include "hello/XComponentNode.h"

#include <GLES3/gl3.h>

namespace hello {

class TextureRenderer : public XComponentNode::Delegate {
 public:
  TextureRenderer();
  ~TextureRenderer() override;

  void RenderTexture(GLenum target,
                     GLuint texture_id,
                     int32_t width,
                     int32_t height,
                     uint64_t timestamp) override;
  void RenderFrame(int32_t width, int32_t height, uint64_t timestamp) override;

 private:
  const uint64_t delta_;
};

}  // namespace hello

#endif  // HELLO_TEXTURE_RENDERER_H
