#ifndef SURFACECONTROL_SURFACECONTROLRENDERNODE_H_
#define SURFACECONTROL_SURFACECONTROLRENDERNODE_H_

#include "core/pipeline/base/render_node.h"

#include <string>

namespace OHOS::Ace {

class SurfaceControlRenderNode : public RenderNode {
  DECLARE_ACE_TYPE(SurfaceControlRenderNode, RenderNode);

 public:
  static RefPtr<RenderNode> Create();

  SurfaceControlRenderNode();
  ~SurfaceControlRenderNode();

 private:
  void Initialize();

  // RenderNode override:
  void Update(const RefPtr<Component>& component) override;
  void SyncGeometryProperties() override;
  void PerformLayout() override;
  void Paint(RenderContext& context, const Offset& offset) override;
  std::shared_ptr<RSNode> CreateRSNode() const override;
  void DumpTree(int32_t depth) override;

  // void OnGlobalPositionChanged() override;

  std::string id_;
};

}  // namespace OHOS::Ace

#endif  // SURFACECONTROL_SURFACECONTROLRENDERNODE_H_
