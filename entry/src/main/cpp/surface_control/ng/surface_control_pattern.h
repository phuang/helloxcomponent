#ifndef SURFACE_CONTROL_PATTERN_H
#define SURFACE_CONTROL_PATTERN_H

#include "core/components_ng/pattern/pattern.h"

namespace OHOS::Ace::NG {

class RenderContext;
class RenderSurface;

class SurfaceControlPattern : public Pattern {
  DECLARE_ACE_TYPE(SurfaceControlPattern, Pattern);

 public:
  SurfaceControlPattern();
  ~SurfaceControlPattern() override;

 private:
  bool IsAtomicNode() const override;
  RefPtr<LayoutProperty> CreateLayoutProperty() override;
  RefPtr<EventHub> CreateEventHub() override;
  RefPtr<LayoutAlgorithm> CreateLayoutAlgorithm() override;
  RefPtr<NodePaintMethod> CreateNodePaintMethod() override;
  FocusPattern GetFocusPattern() const override;
  bool NeedSoftKeyboard() const override;

  void OnAttachToFrameNode() override;
  void OnAttachToMainTree() override;
  void BeforeSyncGeometryProperties(const DirtySwapConfig& config) override;
  void OnDetachFromMainTree() override;
  void OnDetachFromFrameNode(FrameNode* frameNode) override;
  void OnRebuildFrame() override;
  void OnAreaChangedInner() override;
  void OnWindowHide() override;
  void OnWindowShow() override;

  void Initialize();
  void InitializeRenderSurface();
  void InitializeRenderContext();

  const std::string id_;
  RefPtr<RenderSurface> renderSurface_;
  RefPtr<RenderContext> renderContext_;
};

}  // namespace OHOS::Ace::NG

#endif  // SURFACE_CONTROL_PATTERN_H
