//
// Created on 2025-03-08.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#include "hello/DelegatedNodeContent.h"

#include "hello/AVPlayer.h"
#include "hello/Constants.h"
#include "hello/Log.h"

namespace hello {

DelegatedNodeContent::DelegatedNodeContent(
    ArkUI_NodeContentHandle content_handle)
    : NodeContent(content_handle) {
  {
    auto renderer = std::make_unique<BitmapRenderer>(kPictureSkyUri);
    root_node_ = XComponentNode::Create(renderer.get(), "root_view",
                                        XComponentNode::kSoftware);
    root_node_->SetWidthPercent(1);
    root_node_->SetHeightPercent(1);
    child_renderers_.push_back(std::move(renderer));
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
    auto renderer = std::make_unique<AVPlayer>(kVideoURL);
    auto node = XComponentNode::Create(renderer.get(), "child_2",
                                       XComponentNode::kNativeWindow);
    node->SetPosition(36, 250);
    node->SetWidth(kBitmapNodeSize);
    node->SetHeight(kBitmapNodeSize);
    root_node_->AddChild(node.get());
    child_renderers_.push_back(std::move(renderer));
    child_nodes_.push_back(std::move(node));
  }
  {
    auto test_handle = api()->createNode(
        static_cast<ArkUI_NodeType>(ARKUI_NODE_CUSTOM_SPAN + 88));
    FATAL_IF(test_handle == nullptr,
             "createNode(ARKUI_NODE_CUSTOM_SPAN + 80) failed!");
    root_node_->AddChild(test_handle);
  }asd
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

// static
ArkUI_NativeNodeAPI_1* DelegatedNodeContent::api() {
  return XComponentNode::api();
}

}  // namespace hello
