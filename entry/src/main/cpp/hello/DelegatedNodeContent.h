//
// Created on 2025-03-08.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#ifndef HELLOXCOMPONENT_DELEGATEDCONTENT_H
#define HELLOXCOMPONENT_DELEGATEDCONTENT_H

#include "hello/NodeContent.h"

#include <memory>

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

  bool visible_ = false;
  std::unique_ptr<hello::XComponentNode> root_node_;
  std::unique_ptr<hello::XComponentNode> child_node_;
};

}  // namespace hello

#endif  // HELLOXCOMPONENT_DELEGATEDCONTENT_H
