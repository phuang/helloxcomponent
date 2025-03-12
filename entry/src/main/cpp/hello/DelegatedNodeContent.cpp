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
  {
    auto renderer = std::make_unique<BitmapRenderer>(kPictureSkyUri);
    root_node_ = hello::XComponentNode::Create(
      renderer.get(), "root_view", hello::XComponentNode::kSoftware);
    root_node_->SetWidthPercent(1);
    root_node_->SetHeightPercent(1);
    child_renderers_.push_back(std::move(renderer));
  }

#if 0
  {
    auto renderer = std::make_unique<TextureRenderer>();
    auto node = hello::XComponentNode::Create(
        renderer.get(), "child_1", hello::XComponentNode::kEGLSurface);
    node->SetPosition(kEGLSurfaceNodeX, kEGLSurfaceNodeX);
    node->SetSurfaceSize(kEGLSurfaceNodeSize, kEGLSurfaceNodeSize);
    node->SetWidth(kEGLSurfaceNodeSize);
    node->SetHeight(kEGLSurfaceNodeSize);
    root_node_->AddChild(node.get());
    child_renderers_.push_back(std::move(renderer));
    child_nodes_.push_back(std::move(node));
  }
  {
    auto renderer = std::make_unique<BitmapRenderer>(kPictureRiverUri);
    auto node = hello::XComponentNode::Create(
        renderer.get(), "child_2", hello::XComponentNode::kSoftware);
    node->SetPosition(kBitmapNodeX, kBitmapNodeY);
    node->SetSurfaceSize(kEGLSurfaceNodeSize, kEGLSurfaceNodeSize);
    node->SetWidth(kBitmapNodeSize);
    node->SetHeight(kBitmapNodeSize);
    root_node_->AddChild(node.get());
    child_renderers_.push_back(std::move(renderer));
    child_nodes_.push_back(std::move(node));
  }
#endif
  {
    auto renderer = std::make_unique<BitmapRenderer>(kPictureRiverUri);
    auto node = hello::XComponentNode::Create(renderer.get(), "child_2",
                                              hello::XComponentNode::kSoftware);
    node->SetPosition(20, 100);
    // node->SetSurfaceSize(kEGLSurfaceNodeSize, kEGLSurfaceNodeSize);
    node->SetWidth(kBitmapNodeSize);
    node->SetHeight(kBitmapNodeSize);
    root_node_->AddChild(node.get());
    child_renderers_.push_back(std::move(renderer));
    child_nodes_.push_back(std::move(node));
  }
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
