#ifndef HELLO_RENDERER_H
#define HELLO_RENDERER_H

#include <EGL/egl.h>
#include <GLES2/gl2.h>

namespace hello {

class NativeWindow;

class Renderer {
 public:
  virtual ~Renderer() = default;
  // For kSoftware:
  virtual void RenderPixels(void* pixels,
                            int32_t width,
                            int32_t height,
                            int32_t stride,
                            uint64_t timestamp) {}
  // For kEGLSurface:
  virtual void RenderFrame(int32_t width, int32_t height, uint64_t timestamp) {}

  // For kEGLImage:
  virtual void RenderTexture(GLenum target,
                             GLuint texture_id,
                             int32_t width,
                             int32_t height,
                             uint64_t timestamp) {}

  // For kNativeWindow and kSurfaceControl:
  virtual void SetNativeWindow(NativeWindow* native_window) {}
  virtual void StartDrawFrame() {}
  virtual void StopDrawFrame() {}

  // For kSurfaceControl:
  virtual void UpdateSurfaceControl(uint64_t timestamp, uint64_t target_timestamp) {}
};

}  // namespace hello

#endif  // HELLO_RENDERER_H
