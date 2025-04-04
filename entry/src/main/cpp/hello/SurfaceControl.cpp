#include "hello/SurfaceControl.h"

#include "hello/BufferQueue.h"
#include "hello/Constants.h"
#include "hello/Log.h"
#include "hello/NativeBuffer.h"
#include "hello/NativeWindow.h"
#include "hello/Renderer.h"

namespace hello {

// static
std::unique_ptr<SurfaceControl> SurfaceControl::Create(const char* name,
                                                       NativeWindow* parent,
                                                       int32_t width,
                                                       int32_t height,
                                                       Renderer* renderer) {
  LOGE("EEEE SurfaceControl::Create() parent=%{public}p name=%{public}s",
       parent, name);
  if (auto* surface =
          OH_SurfaceControl_FromNativeWindow(parent->window(), name)) {
    return std::unique_ptr<SurfaceControl>(
        new SurfaceControl(surface, width, height, renderer));
  }
  return {};
}

// static
std::unique_ptr<SurfaceControl> SurfaceControl::Create(const char* name,
                                                       int32_t width,
                                                       int32_t height,
                                                       Renderer* renderer) {
  LOGE("EEEE SurfaceControl::Create() name=%{public}s", name);
  if (auto* surface = OH_SurfaceControl_Create(name)) {
    return std::unique_ptr<SurfaceControl>(
        new SurfaceControl(surface, width, height, renderer));
  }
  return {};
}

SurfaceControl::SurfaceControl(OH_SurfaceControl* surface,
                               int32_t width,
                               int32_t height,
                               Renderer* renderer)
    : surface_(surface), renderer_(renderer) {
  buffer_queue_ = BufferQueue::Create(width, height);
}

SurfaceControl::~SurfaceControl() {
  if (surface_) {
    OH_SurfaceControl_Release(surface_);
  }
}

void SurfaceControl::SoftwareDrawFrame() {
  LOGE("EEEE SoftwareDrawFrame called");
  if (renderer_) {
  }
}

void SurfaceControl::HardwareDrawFrame() {
  LOGE("EEEE HardwareDrawFrame called");
  if (renderer_) {
  }
}

void SurfaceControl::OnBufferRelease(int release_fence_fd) {
  LOGE("EEEE Buffer released");
}

// static
void SurfaceControl::OnBufferReleaseStub(void* context, int release_fence_fd) {
  if (context) {
    auto* self = static_cast<SurfaceControl*>(context);
    self->OnBufferRelease(release_fence_fd);
  }
  if (release_fence_fd >= 0) {
    close(release_fence_fd);
  }
}

}  // namespace hello
