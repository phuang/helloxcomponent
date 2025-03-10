#ifndef HELLO_COMPOSITOR_H
#define HELLO_COMPOSITOR_H

#include "hello/XComponentNode.h"

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
  std::unique_ptr<TextureRenderer> texture_renderer_;
  std::unique_ptr<NativeWindow> texture_native_window_;
  std::unique_ptr<BitmapRenderer> bitmap_renderer_;
  std::unique_ptr<NativeWindow> bitmap_native_window_;
};

}  // namespace hello

#endif  // HELLO_COMPOSITOR_H
