
#ifndef SURFACE_CONTROL_SURFACECONTROL_LAYOUT_PROPERTY_H
#define SURFACE_CONTROL_SURFACECONTROL_LAYOUT_PROPERTY_H

#include "core/components_ng/layout/layout_property.h"
#include "core/components_ng/property/property.h"

namespace OHOS::Ace::NG {

class ACE_EXPORT SurfaceControlLayoutProperty : public LayoutProperty {
  DECLARE_ACE_TYPE(SurfaceControlLayoutProperty, LayoutProperty);

 public:
  SurfaceControlLayoutProperty();
  ~SurfaceControlLayoutProperty() override;

  RefPtr<LayoutProperty> Clone() const override;
  void Reset() override;

  // ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(XComponentType, XComponentType,
  // PROPERTY_UPDATE_MEASURE);

 private:
  ACE_DISALLOW_COPY_AND_MOVE(SurfaceControlLayoutProperty);
};
}  // namespace OHOS::Ace::NG

#endif  // SURFACE_CONTROL_SURFACECONTROL_LAYOUT_PROPERTY_H
