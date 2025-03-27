#include "surface_control/surface_control_component.h"

#include "graphic/graphic_2d/rosen/modules/render_service_client/core/ui/rs_surface_node.h"
#include "surface_control/surface_control_render_node.h"
#include "core/components/xcomponent/xcomponent_element.h"

namespace OHOS::Ace {

SurfaceControlComponent::SurfaceControlComponent() = default;

SurfaceControlComponent::~SurfaceControlComponent() = default;

RefPtr<RenderNode> SurfaceControlComponent::CreateRenderNode()
{
    RefPtr<RenderNode> renderNode = SurfaceControlRenderNode::Create();
    // auto surfaceControlRenderNode = AceType::DynamicCast<SurfaceControlRenderNode>(renderNode);
    // surfaceControlRenderNode->SetXComponentId(GetId());
    return renderNode;
}

RefPtr<Element> SurfaceControlComponent::CreateElement()
{
    // return AceType::MakeRefPtr<XComponentElement>();
    return {};
}

}  // namespace OHOS::Ace
