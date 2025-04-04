#include "hello/SurfaceControlNodeContent.h"

#include "hello/AVPlayer.h"
#include "hello/Constants.h"
#include "hello/Log.h"
#include "hello/NativeWindow.h"
#include "hello/SurfaceControl.h"

namespace hello {

SurfaceControlNodeContent::SurfaceControlNodeContent(
    ArkUI_NodeContentHandle content_handle)
    : NodeContent(content_handle) {
  {
    // auto renderer = std::make_unique<BitmapRenderer>(kPictureSkyUri);
    root_node_ = XComponentNode::Create(this, "root_view",
                                        XComponentNode::kNativeWindow);
    root_node_->SetWidthPercent(1);
    root_node_->SetHeightPercent(1);
    // child_renderers_.push_back(std::move(renderer));
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
#endif
}

SurfaceControlNodeContent::~SurfaceControlNodeContent() {
  DetachRootNode();
}

void SurfaceControlNodeContent::SetVisible(bool visible) {
  if (visible_ == visible) {
    return;
  }

  visible_ = visible;
  // if (visible_) {
  //   root_node_->StartDrawFrame();
  //   for (auto& node : child_nodes_) {
  //     node->StartDrawFrame();
  //   }
  // } else {
  //   root_node_->StopDrawFrame();
  //   for (auto& node : child_nodes_) {
  //     node->StopDrawFrame();
  //   }
  // }
}

XComponentNode* SurfaceControlNodeContent::GetRootNode() {
  return root_node_.get();
}

void SurfaceControlNodeContent::OnRootNodeAttached() {}

void SurfaceControlNodeContent::OnRootNodeDetached() {}

void SurfaceControlNodeContent::SetNativeWindow(NativeWindow* native_window) {
  FATAL_IF(root_surface_, "root_surface_ has been created!");
  root_surface_ = SurfaceControl::Create(native_window, "root_surface");
}

void SurfaceControlNodeContent::StartDrawFrame() {}

void SurfaceControlNodeContent::StopDrawFrame() {}

// static
ArkUI_NativeNodeAPI_1* SurfaceControlNodeContent::api() {
  return XComponentNode::api();
}

}  // namespace hello
