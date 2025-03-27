#ifndef SURFACE_CONTROL_PATTERN_H
#define SURFACE_CONTROL_PATTERN_H

#include "core/components_ng/pattern/pattern.h"

namespace OHOS::Ace::NG {

class SurfaceControlPattern : public Pattern {
  DECLARE_ACE_TYPE(SurfaceControlPattern, Pattern);

 public:
  SurfaceControlPattern();
  ~SurfaceControlPattern() override;

  bool IsAtomicNode() const override;
  RefPtr<LayoutProperty> CreateLayoutProperty() override;
  RefPtr<EventHub> CreateEventHub() override;
  RefPtr<LayoutAlgorithm> CreateLayoutAlgorithm() override;
  RefPtr<NodePaintMethod> CreateNodePaintMethod() override;
  FocusPattern GetFocusPattern() const override;
  bool NeedSoftKeyboard() const override;
};

}  // namespace OHOS::Ace::NG

#endif  // SURFACE_CONTROL_PATTERN_H
