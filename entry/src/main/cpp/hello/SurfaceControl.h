#ifndef HELLOXCOMPONENT_SURFACECONTROL_H
#define HELLOXCOMPONENT_SURFACECONTROL_H

#include "hello/NodeContent.h"

#include <memory>
#include <vector>

#include "surface_control/ndk/surface_control.h"

namespace hello {

class BufferQueue;
class Renderer;

class SurfaceControl {
 public:
  static std::unique_ptr<SurfaceControl> Create(const char* name,
                                                NativeWindow* parent,
                                                int32_t width,
                                                int32_t height,
                                                Renderer* renderer);

  static std::unique_ptr<SurfaceControl> Create(const char* name,
                                                int32_t width,
                                                int32_t height,
                                                Renderer* renderer);

  ~SurfaceControl();

 private:
  SurfaceControl(const SurfaceControl&) = delete;
  SurfaceControl(SurfaceControl&&) = delete;
  SurfaceControl& operator=(const SurfaceControl&) = delete;
  SurfaceControl& operator=(SurfaceControl&&) = delete;

  SurfaceControl(OH_SurfaceControl* surface,
                 int32_t width,
                 int32_t height,
                 Renderer* renderer);

  void SoftwareDrawFrame();
  void HardwareDrawFrame();

  static void OnBufferReleaseStub(void* context, int release_fence_fd);
  void OnBufferRelease(int release_fence_fd);

  OH_SurfaceControl* surface_ = nullptr;
  Renderer* const renderer_;
  std::shared_ptr<BufferQueue> buffer_queue_;
};

}  // namespace hello

#endif  // HELLOXCOMPONENT_SURFACECONTROL_H
