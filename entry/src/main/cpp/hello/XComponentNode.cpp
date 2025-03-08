//
// Created on 2025-03-06.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "XComponentNode.h"

#include <arkui/native_interface.h>
#include <native_buffer/native_buffer.h>
#include <native_window/external_window.h>

#include <cassert>
#include <map>
#include <mutex>

#include "common/log.h"

namespace hello {
namespace {
std::map<OH_NativeXComponent*, XComponentNode*> xcomponent_nodes_;
}

// static
std::unique_ptr<XComponentNode> XComponentNode::Create(std::string id, Type type) {
  ArkUI_NodeHandle handle = api()->createNode(ARKUI_NODE_XCOMPONENT);
  if (!handle) {
    return {};
  }
  
  {
    ArkUI_NumberValue value;
    value.i32 = type == kSurface ? ARKUI_XCOMPONENT_TYPE_SURFACE : ARKUI_XCOMPONENT_TYPE_TEXTURE;
    ArkUI_AttributeItem item = {&value, 1}; 
    int32_t retval = api()->setAttribute(handle, NODE_XCOMPONENT_TYPE, &item);
    LOGE("EEEE setAttribute(NODE_XCOMPONENT_TYPE) return %{public}d", retval);
  }
  {
    ArkUI_AttributeItem item = {.string = id.c_str()}; 
    int32_t retval = api()->setAttribute(handle, NODE_XCOMPONENT_ID, &item);
    LOGE("EEEE setAttribute(NODE_XCOMPONENT_ID) return %{public}d", retval);
  }
  return std::unique_ptr<XComponentNode>(new XComponentNode(handle));
}

XComponentNode::XComponentNode(ArkUI_NodeHandle handle)
  : handle_(handle), component_(OH_NativeXComponent_GetNativeXComponent(handle_)) {
  assert(component_);
  xcomponent_nodes_[component_] = this;
  static OH_NativeXComponent_Callback callbacks = {
    [](OH_NativeXComponent* component, void* window) {
      GetInstance(component)->OnSurfaceCreated(window);
    },
    [](OH_NativeXComponent* component, void* window) {
      GetInstance(component)->OnSurfaceChanged(window);
    },
    [](OH_NativeXComponent* component, void* window) {
      GetInstance(component)->OnSurfaceDestroyed(window);
    },
    [](OH_NativeXComponent* component, void* window) {
      GetInstance(component)->DispatchTouchEvent(window);
    },
  };
  int32_t retval = OH_NativeXComponent_RegisterCallback(component_, &callbacks);
  LOGE("EEEE OH_NativeXComponent_RegisterCallback() return %{public}d", retval);
}

XComponentNode::~XComponentNode() {
  xcomponent_nodes_.erase(component_);
  api()->disposeNode(handle_);
}

// static
ArkUI_NativeNodeAPI_1* XComponentNode::api() {
  static ArkUI_NativeNodeAPI_1* api = nullptr;
  static std::once_flag flag;
  std::call_once(flag, [&]() {
    api = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(OH_ArkUI_QueryModuleInterfaceByName(
        ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
  });
  return api;
}

void XComponentNode::SetPosition(float x, float y) {
  ArkUI_NumberValue values[] = {{.f32 = x }, {.f32 = y }};
  ArkUI_AttributeItem item = {values, 2}; 
  api()->setAttribute(handle_, NODE_POSITION, &item); 
}

void XComponentNode::SetWidth(float width) {
  ArkUI_NumberValue value = {.f32 = width };
  ArkUI_AttributeItem item = {&value, 1}; 
  api()->setAttribute(handle_, NODE_WIDTH, &item); 
}

void XComponentNode::SetHeight(float height) {
  ArkUI_NumberValue value = {.f32 = height };
  ArkUI_AttributeItem item = {&value, 1}; 
  api()->setAttribute(handle_, NODE_HEIGHT, &item);   
}

void XComponentNode::SetWidthPercent(float width) {
  ArkUI_NumberValue value = {.f32 = width };
  ArkUI_AttributeItem item = {&value, 1}; 
  api()->setAttribute(handle_, NODE_WIDTH_PERCENT, &item); 
}

void XComponentNode::SetHeightPercent(float height) {
  ArkUI_NumberValue value = {.f32 = height };
  ArkUI_AttributeItem item = {&value, 1}; 
  api()->setAttribute(handle_, NODE_HEIGHT_PERCENT, &item);   
}

void XComponentNode::SetScale(float x, float y) {
  ArkUI_NumberValue values[] = {{.f32 = x }, {.f32 = y }};
  ArkUI_AttributeItem item = {values, 2}; 
  api()->setAttribute(handle_, NODE_SCALE, &item); 
}

void XComponentNode::SetBackgroundColor(uint32_t argb) {
  ArkUI_NumberValue value = {.u32 = argb };
  ArkUI_AttributeItem item = {&value, 1}; 
  api()->setAttribute(handle_, NODE_BACKGROUND_COLOR, &item);   
}

void XComponentNode::SetFocusable(bool focusable) {
  ArkUI_NumberValue value = {.i32 = focusable ? 1 : 0 };
  ArkUI_AttributeItem item = {&value, 1}; 
  api()->setAttribute(handle_, NODE_FOCUSABLE, &item);   
}

void XComponentNode::SetSurfaceSize(uint32_t width, uint32_t height) {
  ArkUI_NumberValue values[] = { {.u32 = width }, { .u32 = height }};
  ArkUI_AttributeItem item = {values, 2 }; 
  int32_t retval = api()->setAttribute(handle_, NODE_XCOMPONENT_SURFACE_SIZE, &item);
  LOGE("EEEE setAttribute(SURFACE_SIZE) return %{public}d", retval);
}

void XComponentNode::OnSurfaceCreated(void* window) {
  LOGE("XComponentNode::%{public}s()", __func__);
  window_ = reinterpret_cast<OHNativeWindow*>(window);
  int32_t retval = OH_NativeXComponent_GetXComponentSize(component_, window_, &surface_width_, &surface_height_);
  LOGE("EEEE OH_NativeXComponent_GetXComponentSize() return %{public}d %{public}dx%{public}d", retval, surface_width_, surface_height_);
  
  OHNativeWindowBuffer* window_buffer = nullptr;
  int fenceFd = -1;
  retval = OH_NativeWindow_NativeWindowRequestBuffer(window_, &window_buffer, &fenceFd);
  LOGE("EEEE OH_NativeWindow_NativeWindowRequestBuffer() return %{public}d fenceFd=%{public}d", retval, fenceFd);
  
  OH_NativeBuffer* buffer = nullptr;
  retval = OH_NativeBuffer_FromNativeWindowBuffer(window_buffer, &buffer);
  
  void* addr = nullptr;
  retval = OH_NativeBuffer_Map(buffer, &addr);
  LOGE("EEEE OH_NativeBuffer_Map() return %{public}d", retval);
  
  uint32_t *pixel = reinterpret_cast<uint32_t*>(addr);
  for (uint64_t y = 0; y < surface_height_; ++y) {
    for (uint64_t x = 0; x < surface_width_; ++x) {
      *pixel = 0x000000ff;
      pixel++;
    }
  }
  
  OH_NativeBuffer_Unmap(buffer);
  retval = OH_NativeWindow_NativeWindowFlushBuffer(window_, window_buffer, -1, {});
}

void XComponentNode::OnSurfaceChanged(void* window) {
  LOGE("XComponentNode::%{public}s()", __func__);
}

void XComponentNode::OnSurfaceDestroyed(void* window) {
  LOGE("XComponentNode::%{public}s()", __func__);
}

void XComponentNode::DispatchTouchEvent(void* window) {
  LOGE("XComponentNode::%{public}s()", __func__);
}

// static
XComponentNode* XComponentNode::GetInstance(OH_NativeXComponent* component) {
  auto it = xcomponent_nodes_.find(component);
  assert(it != xcomponent_nodes_.end());
  return it->second;
}

}  // namespace hello