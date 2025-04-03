#ifndef HELLOXCOMPONENT_NONDELEGATEDNODECONTENT_H
#define HELLOXCOMPONENT_NONDELEGATEDNODECONTENT_H

#include "hello/NodeContent.h"

namespace hello {

class Compositor;
class XComponentNode;

class NonDelegatedNodeContent : public NodeContent {
 public:
  NonDelegatedNodeContent(ArkUI_NodeContentHandle content_handle);
  ~NonDelegatedNodeContent() override;

  void SetVisible(bool visible) override;

 private:
  NonDelegatedNodeContent(const NonDelegatedNodeContent&) = delete;
  NonDelegatedNodeContent& operator=(const NonDelegatedNodeContent&) = delete;
  NonDelegatedNodeContent(NonDelegatedNodeContent&&) = delete;
  NonDelegatedNodeContent& operator=(NonDelegatedNodeContent&&) = delete;

  XComponentNode* GetRootNode() override;
  void OnRootNodeAttached() override;
  void OnRootNodeDetached() override;

  bool visible_ = false;
  std::unique_ptr<XComponentNode> root_node_;
  std::unique_ptr<Compositor> compositor_;
};

}  // namespace hello

#endif  // HELLOXCOMPONENT_NONDELEGATEDNODECONTENT_H
