#include "surface_control/ng/surface_control_model.h"

#include "ace_engine/interfaces/native/node/node_model.h"
#include "core/common/container.h"
#include "core/pipeline/base/element_register.h"
#include "surface_control/log.h"
#include "surface_control/ng/surface_control_layout_property.h"
#include "surface_control/ng/surface_control_pattern.h"

namespace OHOS::Ace::NG {
namespace {

constexpr char SURFACE_CONTROL_TAG[] = "SurfaceControl";

void* CreateNode(ArkUI_NodeType type, ArkUI_Int32 nodeId) {
  if (nodeId == ARKUI_AUTO_GENERATE_NODE_ID) {
    nodeId = ElementRegister::GetInstance()->MakeUniqueId();
  }
  auto frameNode = SurfaceControlModel::CreateFrameNode(nodeId, "");
  frameNode->IncRefCount();
  return AceType::RawPtr(frameNode);
}

}  // namespace

// static
void SurfaceControlModel::SetNodeCreator() {
  OHOS::Ace::NodeModel::SetNodeCreator(
      [](ArkUI_NodeType type, ArkUI_Int32 nodeId, ArkUI_Int32 flags) {
        ArkUINodeHandle node = nullptr;
        if (flags == ARKUI_NODE_FLAG_C) {
        //   ContainerScope Scope(Container::CurrentIdSafelyWithCheck());
          node = reinterpret_cast<ArkUINodeHandle>(CreateNode(type, nodeId));
          auto* uiNode = reinterpret_cast<UINode*>(node);
          if (uiNode) {
            uiNode->setIsCNode(true);
          }
        } else {
          node = reinterpret_cast<ArkUINodeHandle>(CreateNode(type, nodeId));
        }
        return node;
      });
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

// static
void SurfaceControlModel::DisposeNode(ArkUI_NodeHandle node) {}

}  // namespace OHOS::Ace::NG
