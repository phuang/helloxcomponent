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
  // root_bitmap_renderer_ = std::make_unique<BitmapRenderer>(kRootPictureUri);
  root_node_ =
      hello::XComponentNode::Create(root_bitmap_renderer_.get(), "root_view",
                                    hello::XComponentNode::kSoftware);
  root_node_->SetWidthPercent(1);
  root_node_->SetHeightPercent(1);

  child_texture_renderer_ = std::make_unique<TextureRenderer>();
  child_elg_surface_node_ = hello::XComponentNode::Create(
      child_texture_renderer_.get(), "child_texture_view",
      hello::XComponentNode::kEGLSurface);
  root_node_->AddChild(child_elg_surface_node_.get());
  child_elg_surface_node_->SetPosition(kEGLSurfaceNodeX, kEGLSurfaceNodeX);
  child_elg_surface_node_->SetSurfaceSize(kEGLSurfaceNodeSize, kEGLSurfaceNodeSize);
  child_elg_surface_node_->SetWidth(kEGLSurfaceNodeSize);
  child_elg_surface_node_->SetHeight(kEGLSurfaceNodeSize);

  child_bitmap_renderer_ = std::make_unique<BitmapRenderer>(kChildPictureUri);
  child_software_node_ = hello::XComponentNode::Create(
      child_bitmap_renderer_.get(), "child_surface_view",
      hello::XComponentNode::kSoftware);
  root_node_->AddChild(child_software_node_.get());
  child_software_node_->SetPosition(kBitmapNodeX, kBitmapNodeY);
  child_software_node_->SetWidth(kBitmapNodeSize);
  child_software_node_->SetHeight(kBitmapNodeSize);
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
    if (child_software_node_) {
      child_software_node_->StartDrawFrame();
    }
    if (child_elg_surface_node_) {
      child_elg_surface_node_->StartDrawFrame();
    }
  } else {
    root_node_->StopDrawFrame();
    if (child_software_node_) {
      child_software_node_->StopDrawFrame();
    }
    if (child_elg_surface_node_) {
      child_elg_surface_node_->StopDrawFrame();
    }
  }
}

XComponentNode* DelegatedNodeContent::GetRootNode() {
  return root_node_.get();
}

void DelegatedNodeContent::OnRootNodeAttached() {}

void DelegatedNodeContent::OnRootNodeDetached() {}

}  // namespace hello
