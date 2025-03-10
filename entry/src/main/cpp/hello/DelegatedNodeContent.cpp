//
// Created on 2025-03-08.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#include "hello/DelegatedNodeContent.h"

namespace hello {
namespace {
const char kRootPictureUri[] =
    "/data/storage/el1/bundle/entry/resources/resfile/"
    "pexels-janik-butz-5366526.jpg";
const char kChildPictureUri[] =
    "/data/storage/el1/bundle/entry/resources/resfile/"
    "pexels-quang-nguyen-vinh-2166711.jpg";
}  // namespace

DelegatedNodeContent::DelegatedNodeContent(
    ArkUI_NodeContentHandle content_handle)
    : NodeContent(content_handle) {
  root_bitmap_renderer_ = std::make_unique<BitmapRenderer>(kRootPictureUri);
  root_node_ =
      hello::XComponentNode::Create(root_bitmap_renderer_.get(), "root_view",
                                    hello::XComponentNode::kSoftware);
  root_node_->SetWidthPercent(1);
  root_node_->SetHeightPercent(1);

  child_bitmap_renderer_ = std::make_unique<BitmapRenderer>(kChildPictureUri);
  child_surface_node_ = hello::XComponentNode::Create(
      child_bitmap_renderer_.get(), "child_surface_view",
      hello::XComponentNode::kSoftware);
  root_node_->AddChild(child_surface_node_.get());
  child_surface_node_->SetPosition(80, 400);
  child_surface_node_->SetWidth(256);
  child_surface_node_->SetHeight(256);

  child_texture_renderer_ = std::make_unique<TextureRenderer>();
  child_texture_node_ = hello::XComponentNode::Create(
      child_texture_renderer_.get(), "child_texture_view",
      hello::XComponentNode::kEGLSurface);
  root_node_->AddChild(child_texture_node_.get());
  child_texture_node_->SetPosition(80, 80);
  child_texture_node_->SetSurfaceSize(256, 256);
  child_texture_node_->SetWidth(256);
  child_texture_node_->SetHeight(256);
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
    if (child_surface_node_) {
      child_surface_node_->StartDrawFrame();
    }
    if (child_texture_node_) {
      child_texture_node_->StartDrawFrame();
    }
  } else {
    root_node_->StopDrawFrame();
    if (child_surface_node_) {
      child_surface_node_->StopDrawFrame();
    }
    if (child_texture_node_) {
      child_texture_node_->StopDrawFrame();
    }
  }
}

XComponentNode* DelegatedNodeContent::GetRootNode() {
  return root_node_.get();
}

void DelegatedNodeContent::OnRootNodeAttached() {}

void DelegatedNodeContent::OnRootNodeDetached() {}

}  // namespace hello
