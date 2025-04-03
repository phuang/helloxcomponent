#include "hello/SurfaceControl.h"

#include "hello/Constants.h"
#include "hello/Log.h"
#include "hello/NativeWindow.h"

namespace hello {

// static
std::unique_ptr<SurfaceControl> SurfaceControl::Create(NativeWindow* parent,
                                                       const char* name) {
  if (auto* surface =
          OH_SurfaceControl_FromNativeWindow(parent->window(), name)) {
    return std::unique_ptr<SurfaceControl>(new SurfaceControl(surface));
  }
  return {};
}

// static
std::unique_ptr<SurfaceControl> SurfaceControl::Create(const char* name) {
  return std::unique_ptr<SurfaceControl>(
      new SurfaceControl(OH_SurfaceControl_Create(name)));
}

SurfaceControl::SurfaceControl(OH_SurfaceControl* surface)
    : surface_(surface) {}

SurfaceControl::~SurfaceControl() {
  if (surface_) {
    OH_SurfaceControl_Release(surface_);
  }
}

}  // namespace hello
