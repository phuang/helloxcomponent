#include "surface_control/surface_control_element.h"

namespace OHOS::Ace {

SurfaceControlElement::~SurfaceControlElement()
{
}

void SurfaceControlElement::Prepare(const WeakPtr<Element>& parent)
{
    Element::Prepare(parent);
}

void SurfaceControlElement::SetNewComponent(const RefPtr<Component>& newComponent)
{
    if (newComponent == nullptr) {
        Element::SetNewComponent(newComponent);
        return;
    }
    // auto surfaceControl = AceType::DynamicCast<SurfaceControlComponent>(newComponent);
    // if (surfaceControl) {
    //     idStr_ = surfaceControl->GetId();
    //     name_ = surfaceControl->GetName();
    //     Element::SetNewComponent(surfaceControl);
    // }
}

} // namespace OHOS::Ace