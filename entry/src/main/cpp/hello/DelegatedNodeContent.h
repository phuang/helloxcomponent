#ifndef HELLOXCOMPONENT_DELEGATEDCONTENT_H
#define HELLOXCOMPONENT_DELEGATEDCONTENT_H

#include "hello/NodeContent.h"

#include <memory>
#include <vector>

namespace hello {

class Renderer;

class DelegatedNodeContent : public NodeContent {
 public:
  DelegatedNodeContent(ArkUI_NodeContentHandle content_handle);
  ~DelegatedNodeContent() override;

  void SetVisible(bool visible) override;

 private:
  DelegatedNodeContent(const DelegatedNodeContent&) = delete;
  DelegatedNodeContent& operator=(const DelegatedNodeContent&) = delete;
  DelegatedNodeContent(DelegatedNodeContent&&) = delete;
  DelegatedNodeContent& operator=(DelegatedNodeContent&&) = delete;

  XComponentNode* GetRootNode() override;
  void OnRootNodeAttached() override;
  void OnRootNodeDetached() override;

  static ArkUI_NativeNodeAPI_1* api();

  bool visible_ = false;
  std::unique_ptr<XComponentNode> root_node_;
  std::vector<std::unique_ptr<XComponentNode>> child_nodes_;
  std::vector<std::unique_ptr<Renderer>> child_renderers_;
};

}  // namespace hello

#endif  // HELLOXCOMPONENT_DELEGATEDCONTENT_H
