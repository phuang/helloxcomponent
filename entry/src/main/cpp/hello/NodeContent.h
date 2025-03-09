//
// Created on 2025-03-08.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef HELLOXCOMPONENT_NODECONTENT_H
#define HELLOXCOMPONENT_NODECONTENT_H

#include "hello/XComponentNode.h"

#include <arkui/native_node.h>

namespace hello {

class NodeContent {
public:
  explicit NodeContent(ArkUI_NodeContentHandle content_handle);
  virtual ~NodeContent();
  
protected:
  void DetachRootNode();

private:
  virtual XComponentNode* GetRootNode() = 0;
  virtual void OnRootNodeAttached() = 0;
  virtual void OnRootNodeDetached() = 0;

  void OnAttachToWindow();
  void OnDetachToWindow();
  
  const ArkUI_NodeContentHandle content_handle_;
  XComponentNode* root_node_ = nullptr;
};

} // namespace hello
#endif // HELLOXCOMPONENT_NODECONTENT_H
