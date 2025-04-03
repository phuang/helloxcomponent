#ifndef HELLOXCOMPONENT_NODECONTENT_H
#define HELLOXCOMPONENT_NODECONTENT_H

#include "hello/XComponentNode.h"

#include <arkui/native_node.h>

namespace hello {

class NodeContent {
 public:
  explicit NodeContent(ArkUI_NodeContentHandle content_handle);
  virtual ~NodeContent();

  virtual void SetVisible(bool visible) = 0;

 protected:
  void DetachRootNode();

 private:
  NodeContent(const NodeContent&) = delete;
  NodeContent& operator=(const NodeContent&) = delete;
  NodeContent(NodeContent&&) = delete;
  NodeContent& operator=(NodeContent&&) = delete;

  // virtual methods:
  virtual XComponentNode* GetRootNode() = 0;
  virtual void OnRootNodeAttached() = 0;
  virtual void OnRootNodeDetached() = 0;

  void OnAttachToWindow();
  void OnDetachToWindow();

  const ArkUI_NodeContentHandle content_handle_;
  bool visible_ = false;
  XComponentNode* root_node_ = nullptr;
};

}  // namespace hello
#endif  // HELLOXCOMPONENT_NODECONTENT_H
