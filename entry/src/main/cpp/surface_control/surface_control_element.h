#ifndef SURFACE_CONTROL_ELEMENT_H
#define SURFACE_CONTROL_ELEMENT_H

#include "core/pipeline/base/render_element.h"

namespace OHOS::Ace {

class SurfaceControlElement : public RenderElement {
  DECLARE_ACE_TYPE(SurfaceControlElement, RenderElement);

 public:
  ~SurfaceControlElement() override;

 private:
  void Prepare(const WeakPtr<Element>& parent) override;
  void SetNewComponent(const RefPtr<Component>& newComponent) override;

};

}  // namespace OHOS::Ace

#endif  // SURFACE_CONTROL_ELEMENT_H
