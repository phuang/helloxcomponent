#ifndef HELLOXCOMPONENT_DELEGATEDCONTENT_H
#define HELLOXCOMPONENT_DELEGATEDCONTENT_H

#include "hello/NodeContent.h"

#include <memory>
#include <vector>

#include "hello/BitmapRenderer.h"
#include "hello/TextureRenderer.h"

namespace hello {

class DelegatedNodeContent : public NodeContent {
 public:
  DelegatedNodeContent(ArkUI_NodeContentHandle content_handle);
  ~DelegatedNodeContent() override;

  void SetVisible(bool visible) override;

 private:
  XComponentNode* GetRootNode() override;
  void OnRootNodeAttached() override;
  void OnRootNodeDetached() override;

  static ArkUI_NativeNodeAPI_1* api();

  bool visible_ = false;
  std::unique_ptr<XComponentNode> root_node_;
  std::vector<std::unique_ptr<XComponentNode>> child_nodes_;
  std::vector<std::unique_ptr<XComponentNode::Delegate>> child_renderers_;
};

}  // namespace hello

#endif  // HELLOXCOMPONENT_DELEGATEDCONTENT_H
