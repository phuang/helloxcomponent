
#ifndef SURFACE_CONTROL_LAYOUT_ALGORITHM_H
#define SURFACE_CONTROL_LAYOUT_ALGORITHM_H

#include <string>

#include "core/components_ng/layout/box_layout_algorithm.h"
#include "core/components_ng/layout/layout_wrapper.h"

namespace OHOS::Ace::NG {
class PipelineContext;

class ACE_EXPORT SurfaceControlLayoutAlgorithm : public BoxLayoutAlgorithm {
  DECLARE_ACE_TYPE(SurfaceControlLayoutAlgorithm, BoxLayoutAlgorithm);

 public:
  SurfaceControlLayoutAlgorithm();
  ~SurfaceControlLayoutAlgorithm() override;

  std::optional<SizeF> MeasureContent(
      const LayoutConstraintF& contentConstraint,
      LayoutWrapper* layoutWrapper) override;

 private:
  ACE_DISALLOW_COPY_AND_MOVE(SurfaceControlLayoutAlgorithm);
};
}  // namespace OHOS::Ace::NG

#endif  // SURFACE_CONTROL_LAYOUT_ALGORITHM_H
