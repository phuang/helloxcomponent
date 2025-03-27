#include "surface_control/ng/surface_control_pattern.h"

namespace OHOS::Ace::NG {

SurfaceControlPattern::SurfaceControlPattern() = default;

SurfaceControlPattern::~SurfaceControlPattern() = default;

bool SurfaceControlPattern::IsAtomicNode() const {
  return true;
}

RefPtr<LayoutProperty> SurfaceControlPattern::CreateLayoutProperty() {
  // return MakeRefPtr<XComponentLayoutProperty>();
  return nullptr;
}

RefPtr<EventHub> SurfaceControlPattern::CreateEventHub() {
  // return MakeRefPtr<XComponentEventHub>();
  return nullptr;
}

RefPtr<LayoutAlgorithm> SurfaceControlPattern::CreateLayoutAlgorithm() {
  // return MakeRefPtr<XComponentLayoutAlgorithm>();
  return nullptr;
}

RefPtr<NodePaintMethod> SurfaceControlPattern::CreateNodePaintMethod() {
  // auto paint =
  //     MakeRefPtr<XComponentPaintMethod>(renderSurface_,
  //     AceType::Claim(this));
  // return paint;
  return nullptr;
}

FocusPattern SurfaceControlPattern::GetFocusPattern() const {
  return {FocusType::DISABLE, true};
}

bool SurfaceControlPattern::NeedSoftKeyboard() const {
  return false;
}

}  // namespace OHOS::Ace::NG
