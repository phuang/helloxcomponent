#include "surface_control/ng/surface_control_pattern.h"

#include "surface_control/ng/surface_control_layout_algorithm.h"
#include "surface_control/ng/surface_control_layout_property.h"
#include "surface_control/ng/surface_control_paint_method.h"
#include "surface_control/log.h"

namespace OHOS::Ace::NG {

SurfaceControlPattern::SurfaceControlPattern() {
  LOGE("EEEE SurfaceControlPattern::SurfaceControlPattern");
}

SurfaceControlPattern::~SurfaceControlPattern()  {
  LOGE("EEEE SurfaceControlPattern::~SurfaceControlPattern");
}

bool SurfaceControlPattern::IsAtomicNode() const {
  return true;
}

RefPtr<LayoutProperty> SurfaceControlPattern::CreateLayoutProperty() {
  return MakeRefPtr<SurfaceControlLayoutProperty>();
}

RefPtr<EventHub> SurfaceControlPattern::CreateEventHub() {
  // return MakeRefPtr<XComponentEventHub>();
  return nullptr;
}

RefPtr<LayoutAlgorithm> SurfaceControlPattern::CreateLayoutAlgorithm() {
  return MakeRefPtr<SurfaceControlLayoutAlgorithm>();
}

RefPtr<NodePaintMethod> SurfaceControlPattern::CreateNodePaintMethod() {
  auto paint = MakeRefPtr<SurfaceControlPaintMethod>(AceType::Claim(this));
  return paint;
}

FocusPattern SurfaceControlPattern::GetFocusPattern() const {
  return {FocusType::DISABLE, true};
}

bool SurfaceControlPattern::NeedSoftKeyboard() const {
  return false;
}

}  // namespace OHOS::Ace::NG
