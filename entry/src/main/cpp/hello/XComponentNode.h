//
// Created on 2025-03-06.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef HELLOXCOMPONENT_XCOMPONENTNODE_H
#define HELLOXCOMPONENT_XCOMPONENTNODE_H

#include <ace/xcomponent/native_interface_xcomponent.h>
#include <arkui/native_node.h>

#include <memory>
#include <string>

class XComponentNode {
public:
  enum Type {
    kSurface,
    kTexture
  };
  
  static std::unique_ptr<XComponentNode> Create(std::string id, Type type);
  ~XComponentNode();

  void SetPosition(float x, float y);
  void SetWidth(float width);
  void SetHeight(float height);
  void SetWidthPercent(float width);
  void SetHeightPercent(float height);
  void SetScale(float x, float y);
  void SetBackgroundColor(uint32_t argb);
  void SetFocusable(bool focusable);

  void SetSurfaceSize(uint32_t width, uint32_t height);
  
private:
  explicit XComponentNode(ArkUI_NodeHandle handle);
  
  static ArkUI_NativeNodeAPI_1* api();
  
  const ArkUI_NodeHandle handle_;
  const OH_NativeXComponent* x_component_;
};

#endif //HELLOXCOMPONENT_XCOMPONENTNODE_H
