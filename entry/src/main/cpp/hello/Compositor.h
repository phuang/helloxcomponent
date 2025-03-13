#ifndef HELLO_COMPOSITOR_H
#define HELLO_COMPOSITOR_H

#include "hello/XComponentNode.h"

#include <GLES3/gl3.h>
#include <native_buffer/native_buffer.h>
#include <native_window/external_window.h>

#include <memory>

#include "hello/GLImage.h"
#include "hello/GLTexture.h"
#include "hello/NativeWindow.h"

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
  void RenderFrameWithTexture(int32_t width,
                              int32_t height,
                              uint64_t timestamp);
  void UploadTextures(int32_t width, int32_t height, uint64_t timestamp);
  void UploadNativeWindows(int32_t width, int32_t height, uint64_t timestamp);
  void UploadNativeBuffers(int32_t width, int32_t height, uint64_t timestamp);

  std::unique_ptr<BitmapRenderer> renderers_[2];
  GLTexture textures_[2];
  std::unique_ptr<NativeWindow> native_windows_[2];
  std::unique_ptr<GLImage> gl_images_[2];
  OH_NativeBuffer* buffers_[2];
  OHNativeWindowBuffer* window_buffers_[2];
};

}  // namespace hello

#endif  // HELLO_COMPOSITOR_H
