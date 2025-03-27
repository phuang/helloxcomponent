#ifndef SURFACE_CONTROL_MODEL_H__
#define SURFACE_CONTROL_MODEL_H__

#include <stddef.h>

#include <string>

#include "base/memory/referenced.h"
#include "core/components_ng/base/frame_node.h"

namespace OHOS::Ace::NG {

class SurfaceControlModel {
 public:
  static RefPtr<FrameNode> CreateFrameNode(int32_t nodeId,
                                           const std::string& id);
};

}  // namespace OHOS::Ace::NG

#endif  // SURFACE_CONTROL_MODEL_H__
