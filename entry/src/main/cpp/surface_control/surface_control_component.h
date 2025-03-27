#ifndef SURFACECONTROL_SURFACECONTROL_H_
#define SURFACECONTROL_SURFACECONTROL_H_

#include "core/pipeline/base/render_component.h"

#include "core/pipeline/base/element.h"

namespace OHOS::Ace {

class RenderXComponent;

class SurfaceControlComponent : public RenderComponent {
  DECLARE_ACE_TYPE(SurfaceControlComponent, RenderComponent);

 public:
  SurfaceControlComponent();
  ~SurfaceControlComponent();

  const std::string GetId() const { return "id"; }

  RefPtr<RenderNode> CreateRenderNode() override;
  RefPtr<Element> CreateElement() override;
};

}  // namespace OHOS::Ace

#endif  // SURFACECONTROL_SURFACECONTROL_H_
