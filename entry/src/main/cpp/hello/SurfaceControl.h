#ifndef HELLOXCOMPONENT_SURFACECONTROL_H
#define HELLOXCOMPONENT_SURFACECONTROL_H

#include "hello/NodeContent.h"

#include <memory>
#include <vector>

#include "hello/BitmapRenderer.h"
#include "hello/TextureRenderer.h"
#include "surface_control/ndk/surface_control.h"

namespace hello {

class SurfaceControl {
 public:
  static std::unique_ptr<SurfaceControl> Create(NativeWindow* parent,
                                                const char* name);
  static std::unique_ptr<SurfaceControl> Create(const char* name);

  ~SurfaceControl();

 private:
  explicit SurfaceControl(OH_SurfaceControl* surface);

  SurfaceControl(const SurfaceControl&) = delete;
  SurfaceControl(SurfaceControl&&) = delete;
  SurfaceControl& operator=(const SurfaceControl&) = delete;
  SurfaceControl& operator=(SurfaceControl&&) = delete;

  OH_SurfaceControl* surface_ = nullptr;
};

}  // namespace hello

#endif  // HELLOXCOMPONENT_SURFACECONTROL_H
