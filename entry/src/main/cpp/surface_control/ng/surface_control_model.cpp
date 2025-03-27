#include "surface_control/ng/surface_control_model.h"

#include "surface_control/ng/surface_control_layout_property.h"
#include "surface_control/ng/surface_control_pattern.h"

namespace OHOS::Ace::NG {
namespace {
constexpr char SURFACE_CONTROL_TAG[] = "SurfaceControl";
}

// static
RefPtr<FrameNode> SurfaceControlModel::CreateFrameNode(int32_t nodeId,
                                                       const std::string& id) {
  auto pattern = AceType::MakeRefPtr<SurfaceControlPattern>();
  auto frameNode =
      FrameNode::CreateFrameNode(SURFACE_CONTROL_TAG, nodeId, pattern);
  auto layoutProperty =
      frameNode->GetLayoutProperty<SurfaceControlLayoutProperty>();
  CHECK_NULL_RETURN(layoutProperty, frameNode);
  // layoutProperty->UpdateXComponentType(type);
  return frameNode;
}

}  // namespace OHOS::Ace::NG
