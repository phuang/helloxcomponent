//
// Created on 2025-03-06.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "XComponentNode.h"

#include <arkui/native_interface.h>

#include <mutex>

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
    api()->setAttribute(handle, NODE_XCOMPONENT_TYPE, &item);
  }
  {
    ArkUI_AttributeItem item = {.string = id.c_str()}; 
    api()->setAttribute(handle, NODE_XCOMPONENT_ID, &item);
  }
  return std::unique_ptr<XComponentNode>(new XComponentNode(handle));
}

XComponentNode::XComponentNode(ArkUI_NodeHandle handle)
  : handle_(handle), x_component_(OH_NativeXComponent_GetNativeXComponent(handle_)) {}

XComponentNode::~XComponentNode() {
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
  ArkUI_AttributeItem item = {values, 2}; 
  api()->setAttribute(handle_, NODE_XCOMPONENT_SURFACE_SIZE, &item);
}
