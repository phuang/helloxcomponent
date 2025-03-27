#include "surface_control/ng/surface_control_layout_algorithm.h"

#include "base/utils/utils.h"
#include "core/components_ng/pattern/linear_layout/linear_layout_utils.h"
#include "surface_control/ng/surface_control_layout_property.h"

namespace OHOS::Ace::NG {

SurfaceControlLayoutAlgorithm::SurfaceControlLayoutAlgorithm() = default;
SurfaceControlLayoutAlgorithm::~SurfaceControlLayoutAlgorithm() = default;

std::optional<SizeF> SurfaceControlLayoutAlgorithm::MeasureContent(
    const LayoutConstraintF& contentConstraint,
    LayoutWrapper* layoutWrapper) {
  if (contentConstraint.selfIdealSize.IsValid()) {
    return contentConstraint.selfIdealSize.ConvertToSizeT();
  }
  return contentConstraint.maxSize;
}

}  // namespace OHOS::Ace::NG
