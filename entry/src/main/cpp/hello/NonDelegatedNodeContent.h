//
// Created on 2025-03-08.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

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
  XComponentNode* GetRootNode() override;
  void OnRootNodeAttached() override;
  void OnRootNodeDetached() override;

  bool visible_ = false;
  std::unique_ptr<XComponentNode> root_node_;
  std::unique_ptr<Compositor> compositor_;
};

}  // namespace hello

#endif  // HELLOXCOMPONENT_NONDELEGATEDNODECONTENT_H
