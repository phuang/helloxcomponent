#include "surface_control/ng/surface_control_paint_method.h"

#include "surface_control/ng/surface_control_pattern.h"

namespace OHOS::Ace::NG {

SurfaceControlPaintMethod::SurfaceControlPaintMethod(
    const RefPtr<SurfaceControlPattern>& pattern)
    : pattern_(pattern) {}

SurfaceControlPaintMethod::~SurfaceControlPaintMethod() = default;

}  // namespace OHOS::Ace::NG
