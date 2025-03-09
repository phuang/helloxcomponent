//
// Created on 2025-03-08.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "hello/NodeContent.h"

#include "common/log.h"

namespace hello {

NodeContent::NodeContent(ArkUI_NodeContentHandle content_handle) : content_handle_(content_handle) {
  FATAL_IF(!hello::XComponentNode::api(), "hello::XComponentNode::api() failed");
  
  int32_t retval = OH_ArkUI_NodeContent_SetUserData(content_handle, this);
  FATAL_IF(retval != ARKUI_ERROR_CODE_NO_ERROR, "OH_ArkUI_NodeContent_SetUserData() failed");

  auto node_content_callback = [](ArkUI_NodeContentEvent *event) {
    ArkUI_NodeContentHandle content_handle = OH_ArkUI_NodeContentEvent_GetNodeContentHandle(event);
    auto *self = reinterpret_cast<NodeContent *>(OH_ArkUI_NodeContent_GetUserData(content_handle));
    switch (OH_ArkUI_NodeContentEvent_GetEventType(event)) {
    case NODE_CONTENT_EVENT_ON_ATTACH_TO_WINDOW:
      self->OnAttachToWindow();
      break;
    case NODE_CONTENT_EVENT_ON_DETACH_FROM_WINDOW:
      self->OnDetachToWindow();
      break;
    }
  };

  retval = OH_ArkUI_NodeContent_RegisterCallback(content_handle, node_content_callback);
  FATAL_IF(retval != ARKUI_ERROR_CODE_NO_ERROR, "OH_ArkUI_NodeContent_RegisterCallback() failed");
}

NodeContent::~NodeContent() {
  CHECK(!root_node_);
}

void NodeContent::DetachRootNode() {
  if (root_node_) {
    OH_ArkUI_NodeContent_RemoveNode(content_handle_, root_node_->handle());
    root_node_ = nullptr;
    OnRootNodeDetached();
  }
}

void NodeContent::OnAttachToWindow() {
  if (!root_node_) {
    root_node_ = GetRootNode();
    OH_ArkUI_NodeContent_AddNode(content_handle_, root_node_->handle());
    OnRootNodeAttached();
  }
}

void NodeContent::OnDetachToWindow() {
  DetachRootNode();
}

} // namespace hello