#include "surface_control/ng/surface_control_layout_property.h"

namespace OHOS::Ace::NG {

SurfaceControlLayoutProperty::SurfaceControlLayoutProperty() = default;
SurfaceControlLayoutProperty::~SurfaceControlLayoutProperty() = default;

RefPtr<LayoutProperty> SurfaceControlLayoutProperty::Clone() const {
  auto value = MakeRefPtr<SurfaceControlLayoutProperty>();
  value->LayoutProperty::UpdateLayoutProperty(
      DynamicCast<LayoutProperty>(this));
  // value->propXComponentType_ = CloneXComponentType();
  return value;
}

void SurfaceControlLayoutProperty::Reset() {
  LayoutProperty::Reset();
  // ResetXComponentType();
}

}  // namespace OHOS::Ace::NG
