#ifndef HELLO_COMPOSITOR_H
#define HELLO_COMPOSITOR_H

#include "hello/XComponentNode.h"

#include <GLES3/gl3.h>

#include <memory>

namespace hello {

class BitmapRenderer;
class NativeWindow;
class TextureRenderer;

class Compositor : public XComponentNode::Delegate {
 public:
  Compositor();
  ~Compositor() override;

  void RenderFrame(int32_t width, int32_t height, uint64_t timestamp) override;

 private:
  std::unique_ptr<BitmapRenderer> renderers_[2];
  GLuint textures_[2];
};

}  // namespace hello

#endif  // HELLO_COMPOSITOR_H
