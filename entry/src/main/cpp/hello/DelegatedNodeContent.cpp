//
// Created on 2025-03-08.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#include "hello/DelegatedNodeContent.h"

#include "hello/Constants.h"

namespace hello {

DelegatedNodeContent::DelegatedNodeContent(
    ArkUI_NodeContentHandle content_handle)
    : NodeContent(content_handle) {
  // TODO: Fix kEGLImage mode. It doesn't render and
  // eglDupNativeFenceFDANDROID() returns -1 with EGL_BAD_DISPLAY constexpr
  constexpr XComponentNode::Type type = XComponentNode::kEGLImage;
  // constexpr XComponentNode::Type type = XComponentNode::kEGLSurface;    
  auto renderer = std::make_unique<TextureRenderer>();

  root_node_ = XComponentNode::Create(renderer.get(), "root_view",
                                      type);
  root_node_->SetWidthPercent(1);
  root_node_->SetHeightPercent(1);
  child_renderers_.push_back(std::move(renderer));
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
    root_node_->StartDrawFrame();
    for (auto& node : child_nodes_) {
      node->StartDrawFrame();
    }
  } else {
    root_node_->StopDrawFrame();
    for (auto& node : child_nodes_) {
      node->StopDrawFrame();
    }
  }
}

XComponentNode* DelegatedNodeContent::GetRootNode() {
  return root_node_.get();
}

void DelegatedNodeContent::OnRootNodeAttached() {}

void DelegatedNodeContent::OnRootNodeDetached() {}

}  // namespace hello
