#include "napi_manager.h"

#include <ace/xcomponent/native_interface_xcomponent.h>
#include <arkui/native_node.h>
#include <arkui/native_node_napi.h>

#include <cassert>
#include <cstdint>
#include <string>

#include "common/log.h"
#include "hello/XComponentNode.h"

namespace helloxcomponent {

// static
NapiManager *NapiManager::GetInstance() {
  static NapiManager manager;
  return &manager;
}

NapiManager::NapiManager() = default;
NapiManager::~NapiManager() = default;


// static
Napi::Value NapiManager::NapiCreateNativeNode(const Napi::CallbackInfo &info) {
  LOGE("NapiManager::NapiCreateNativeNode()");
  Napi::Env env = info.Env();

  if (info.Length() != 1) {
    Napi::Error::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
    return env.Null();
  }

  ArkUI_NodeContentHandle content_handle = nullptr;
  int32_t retval = OH_ArkUI_GetNodeContentFromNapiValue(env, info[0], &content_handle);
  if (retval != ARKUI_ERROR_CODE_NO_ERROR) {
    LOGE("OH_ArkUI_GetNodeContentFromNapiValue() failed");
    Napi::Error::New(env, "The first arg is not NodeContent").ThrowAsJavaScriptException();
    return env.Null();
  }

  GetInstance()->CreateNativeNode(content_handle);

  return env.Null();
}

void NapiManager::CreateNativeNode(ArkUI_NodeContentHandle content_handle) {
  if (root_node_) {
    return;
  }

  if (!hello::XComponentNode::api()) {
    LOGE("hello::XComponentNode::api() is nullptr");
  }

  int32_t retval = OH_ArkUI_NodeContent_SetUserData(content_handle, this);
  if (retval != ARKUI_ERROR_CODE_NO_ERROR) {
    LOGE("OH_ArkUI_NodeContent_SetUserData() failed");
  }

  auto node_content_callback = [](ArkUI_NodeContentEvent *event) {
    LOGE("NapiManager::CreateNativeNode() node_content_callback()");
    ArkUI_NodeContentHandle content_handle = OH_ArkUI_NodeContentEvent_GetNodeContentHandle(event);
    auto *self = reinterpret_cast<NapiManager *>(OH_ArkUI_NodeContent_GetUserData(content_handle));
    auto event_type = OH_ArkUI_NodeContentEvent_GetEventType(event);
    LOGE("EEEE event_type=%{public}d", event_type);
    if (OH_ArkUI_NodeContentEvent_GetEventType(event) == NODE_CONTENT_EVENT_ON_ATTACH_TO_WINDOW) {
      assert(!self->root_node_);
      auto root_node = hello::XComponentNode::Create("root_view", hello::XComponentNode::kSurface);
      root_node->SetWidthPercent(1);
      root_node->SetHeightPercent(1);

      auto child_node = hello::XComponentNode::Create("child_view", hello::XComponentNode::kSurface);
      root_node->AddChild(child_node.get());
      child_node->SetPosition(40, 200);
      child_node->SetWidth(512);
      child_node->SetHeight(512);
//      
      OH_ArkUI_NodeContent_AddNode(content_handle, root_node->handle());
      self->root_node_ = std::move(root_node);
      self->child_node_ = std::move(child_node);
    }
  };

  retval = OH_ArkUI_NodeContent_RegisterCallback(content_handle, node_content_callback);
  if (retval != ARKUI_ERROR_CODE_NO_ERROR) {
    LOGE("OH_ArkUI_NodeContent_RegisterCallback() failed");
  }
}

} // namespace helloxcomponent
