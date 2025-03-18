//
// Created on 2025-03-08.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#include "hello/DelegatedNodeContent.h"

#include "hello/AVPlayer.h"
#include "hello/Constants.h"
#include "hello/DisplayManager.h"
#include "hello/NapiManager.h"

namespace hello {

DelegatedNodeContent::DelegatedNodeContent(
    ArkUI_NodeContentHandle content_handle)
    : NodeContent(content_handle) {
  {
    XComponentNode::Delegate* delegate = nullptr;
    // auto renderer = std::make_unique<BitmapRenderer>(kPictureSkyUri);
    // delegate = renderer.get();
    // child_renderers_.push_back(std::move(renderer));

    root_node_ = XComponentNode::Create(delegate, "root_view",
                                        XComponentNode::kSoftware);
    root_node_->SetWidthPercent(1);
    root_node_->SetHeightPercent(1);
  }

#if 0
  {
    // TODO: Fix kEGLImage mode. It doesn't render and
    // eglDupNativeFenceFDANDROID() returns -1 with EGL_BAD_DISPLAY
    constexpr XComponentNode::Type type = XComponentNode::kEGLImage;

    // constexpr XComponentNode::Type type = XComponentNode::kEGLSurface;

    auto renderer = std::make_unique<TextureRenderer>();
    auto node = XComponentNode::Create(renderer.get(), "child_1", type);
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
    auto node = XComponentNode::Create(renderer.get(), "child_3",
                                       XComponentNode::kSoftware);
    node->SetPosition(kBitmapNodeX, kBitmapNodeY);
    node->SetSurfaceSize(kEGLSurfaceNodeSize, kEGLSurfaceNodeSize);
    node->SetWidth(kBitmapNodeSize);
    node->SetHeight(kBitmapNodeSize);
    root_node_->AddChild(node.get());
    child_renderers_.push_back(std::move(renderer));
    child_nodes_.push_back(std::move(node));
  }
#else
  // {
  //   auto renderer = std::make_unique<BitmapRenderer>(kPictureRiverUri);
  //   auto node = XComponentNode::Create(renderer.get(), "child_2",
  //                                      XComponentNode::kSoftware);
  //   node->SetPosition(36, 250);
  //   node->SetWidth(kBitmapNodeSize);
  //   node->SetHeight(kBitmapNodeSize);
  //   root_node_->AddChild(node.get());
  //   child_renderers_.push_back(std::move(renderer));
  //   child_nodes_.push_back(std::move(node));
  // }
  {
    float scaled_density =
        NapiManager::GetInstance()->display_manager()->scaled_density();
    float video_width = kVideoNodeWidth;
    float video_height = kVideoNodeHeight;
    float scaled_video_width = scaled_density * video_width;
    float scaled_video_height = scaled_density * video_height;

    auto renderer = std::make_unique<AVPlayer>(kVideoURL);
    auto node = XComponentNode::Create(renderer.get(), "child_video",
                                       XComponentNode::kNativeWindow);
    node->SetPosition(0, 0);
    node->SetWidth(kVideoNodeWidth);
    node->SetHeight(kVideoNodeHeight);
    node->SetRotate(0, 0, 1, 90, 0);
    float scale_x = kWindowHeight / scaled_video_width;
    float scale_y = kWindowWidth / scaled_video_height;
    node->SetScale(scale_x, scale_y);
    node->SetTranslate(-video_width / 2 + kWindowWidth / scaled_density / 2,
                       -video_height / 2 + kWindowHeight / scaled_density / 2,
                       0);
    root_node_->AddChild(node.get());
    child_renderers_.push_back(std::move(renderer));
    child_nodes_.push_back(std::move(node));
  }
#endif
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
