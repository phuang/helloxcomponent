//
// Created on 2025-03-08.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#include "hello/NonDelegatedNodeContent.h"

#include "hello/Compositor.h"
#include "hello/XComponentNode.h"

namespace hello {
namespace {}  // namespace

NonDelegatedNodeContent::NonDelegatedNodeContent(
    ArkUI_NodeContentHandle content_handle)
    : NodeContent(content_handle) {
  compositor_ = std::make_unique<Compositor>();
  root_node_ = XComponentNode::Create(compositor_.get(), "root_view",
                                      XComponentNode::kEGLSurface);
  root_node_->SetWidthPercent(1);
  root_node_->SetHeightPercent(1);
}

NonDelegatedNodeContent::~NonDelegatedNodeContent() {
  DetachRootNode();
}

void NonDelegatedNodeContent::SetVisible(bool visible) {
  if (visible_ == visible) {
    return;
  }

  visible_ = visible;
  if (visible_) {
    root_node_->StartDrawFrame();
  } else {
    root_node_->StopDrawFrame();
  }
}

XComponentNode* NonDelegatedNodeContent::GetRootNode() {
  return root_node_.get();
}

void NonDelegatedNodeContent::OnRootNodeAttached() {}

void NonDelegatedNodeContent::OnRootNodeDetached() {}

}  // namespace hello
