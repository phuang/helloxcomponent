#include "hello/SurfaceControlNodeContent.h"

#include "hello/BitmapRenderer.h"
#include "hello/Constants.h"
#include "hello/Log.h"
#include "hello/NativeWindow.h"
#include "hello/SurfaceControl.h"
#include "hello/TextureRenderer.h"

namespace hello {

SurfaceControlNodeContent::SurfaceControlNodeContent(
    ArkUI_NodeContentHandle content_handle)
    : NodeContent(content_handle) {
  // Use an XComponentNode as the root node, and OH_SurfaceControl wiil use it
  // as parent.
  root_node_ = XComponentNode::Create(this, "root_view",
                                      XComponentNode::kSurfaceControl);
  root_node_->SetWidthPercent(1);
  root_node_->SetHeightPercent(1);
}

SurfaceControlNodeContent::~SurfaceControlNodeContent() {
  DetachRootNode();
}

void SurfaceControlNodeContent::SetVisible(bool visible) {
  if (visible_ == visible) {
    return;
  }

  visible_ = visible;
  if (visible_) {
    root_node_->StartDrawFrame();
    // for (auto& node : child_nodes_) {
    //   node->StartDrawFrame();
    // }
  } else {
    root_node_->StopDrawFrame();
    // for (auto& node : child_nodes_) {
    //   node->StopDrawFrame();
    // }
  }
}

XComponentNode* SurfaceControlNodeContent::GetRootNode() {
  return root_node_.get();
}

void SurfaceControlNodeContent::OnRootNodeAttached() {}

void SurfaceControlNodeContent::OnRootNodeDetached() {}

void SurfaceControlNodeContent::SetNativeWindow(NativeWindow* native_window) {
  LOGE("EEEE SurfaceControlNodeContent::SetNativeWindow()");
  FATAL_IF(root_surface_, "root_surface_ has been created!");
  auto renderer = std::make_unique<BitmapRenderer>(kPictureSkyUri);
  root_surface_ = SurfaceControl::Create("root_surface", native_window,
                                         kWindowWidth, kWindowHeight,
                                         /*is_software=*/true, renderer.get());
  renderers_.push_back(std::move(renderer));
}

void SurfaceControlNodeContent::StartDrawFrame() {
  LOGE("EEEE SurfaceControlNodeContent::StartDrawFrame()");
}

void SurfaceControlNodeContent::StopDrawFrame() {
  LOGE("EEEE SurfaceControlNodeContent::StopDrawFrame()");
}

void SurfaceControlNodeContent::UpdateSurfaceControl(
    uint64_t timestamp,
    uint64_t target_timestamp) {
  if (!root_surface_) {
    return;
  }

  OH_SurfaceTransaction* transaction = OH_SurfaceTransaction_Create();
  bool need_commit =
      root_surface_->Update(transaction, timestamp, target_timestamp);
  for (const auto& surface : child_surfaces_) {
    need_commit |= surface->Update(transaction, timestamp, target_timestamp);
  }
  if (need_commit) {
    OH_SurfaceTransaction_SetDesiredPresentTime(transaction, timestamp);
    OH_SurfaceTransaction_Commit(transaction);
  }
  OH_SurfaceTransaction_Delete(transaction);
}

}  // namespace hello
