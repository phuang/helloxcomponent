//
// Created on 2025-03-08.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#include "hello/DelegatedNodeContent.h"

namespace hello {

DelegatedNodeContent::DelegatedNodeContent(
    ArkUI_NodeContentHandle content_handle)
    : NodeContent(content_handle) {
  root_node_ = hello::XComponentNode::Create("root_view",
                                             hello::XComponentNode::kSurface);
  root_node_->SetWidthPercent(1);
  root_node_->SetHeightPercent(1);

  child_node_ = hello::XComponentNode::Create("child_view",
                                              hello::XComponentNode::kSurface);
  root_node_->AddChild(child_node_.get());
  child_node_->SetPosition(40, 200);
  child_node_->SetWidth(512);
  child_node_->SetHeight(512);
}

DelegatedNodeContent::~DelegatedNodeContent() {
  DetachRootNode();
}

void DelegatedNodeContent::SetVisible(bool visible) {
  if (visible_ == visible) {
    return;
  }

  visible_ = visible;
  if (visible_) {
  } else {
  }
}

XComponentNode* DelegatedNodeContent::GetRootNode() {
  return root_node_.get();
}

void DelegatedNodeContent::OnRootNodeAttached() {}

void DelegatedNodeContent::OnRootNodeDetached() {}

}  // namespace hello
