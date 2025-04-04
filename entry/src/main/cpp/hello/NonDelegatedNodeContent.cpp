#include "hello/NonDelegatedNodeContent.h"

#include "hello/Compositor.h"
#include "hello/Log.h"
#include "hello/XComponentNode.h"

namespace hello {
namespace {}  // namespace

NonDelegatedNodeContent::NonDelegatedNodeContent(
    ArkUI_NodeContentHandle content_handle)
    : NodeContent(content_handle) {
  compositor_ = std::make_unique<Compositor>();
  root_node_ = XComponentNode::Create(compositor_.get(), "root_view",
                                      XComponentNode::kSurfaceControl);
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
