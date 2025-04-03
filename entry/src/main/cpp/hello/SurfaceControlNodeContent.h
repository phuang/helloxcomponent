#ifndef HELLOXCOMPONENT_SURFACECONTROLNODECONTENT_H
#define HELLOXCOMPONENT_SURFACECONTROLNODECONTENT_H

#include "hello/NodeContent.h"

#include <memory>
#include <vector>

#include "hello/BitmapRenderer.h"
#include "hello/TextureRenderer.h"
#include "surface_control/ndk/surface_control.h"

namespace hello {

class SurfaceControl;

class SurfaceControlNodeContent : public NodeContent,
                                  public XComponentNode::Delegate {
 public:
  SurfaceControlNodeContent(ArkUI_NodeContentHandle content_handle);
  ~SurfaceControlNodeContent() override;

 private:
  SurfaceControlNodeContent(const SurfaceControlNodeContent&) = delete;
  SurfaceControlNodeContent& operator=(const SurfaceControlNodeContent&) =
      delete;
  SurfaceControlNodeContent(SurfaceControlNodeContent&&) = delete;
  SurfaceControlNodeContent& operator=(SurfaceControlNodeContent&&) = delete;

  // NodeContent override:
  void SetVisible(bool visible) override;
  XComponentNode* GetRootNode() override;
  void OnRootNodeAttached() override;
  void OnRootNodeDetached() override;

  // XComponentNode::Delegate override:
  void SetNativeWindow(NativeWindow* native_window) override;
  void StartDrawFrame() override;
  void StopDrawFrame() override;

  static ArkUI_NativeNodeAPI_1* api();

  bool visible_ = false;
  std::unique_ptr<XComponentNode> root_node_;
  // std::vector<std::unique_ptr<XComponentNode>> child_nodes_;
  // std::vector<std::unique_ptr<XComponentNode::Delegate>> child_renderers_;
  std::unique_ptr<SurfaceControl> root_surface_;
};

}  // namespace hello

#endif  // HELLOXCOMPONENT_SURFACECONTROLNODECONTENT_H
