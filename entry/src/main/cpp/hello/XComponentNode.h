//
// Created on 2025-03-06.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef HELLOXCOMPONENT_XCOMPONENTNODE_H
#define HELLOXCOMPONENT_XCOMPONENTNODE_H

#include <ace/xcomponent/native_interface_xcomponent.h>
#include <arkui/native_node.h>
#include <native_window/external_window.h>

#include <memory>
#include <string>

namespace hello {

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
  
  ArkUI_NodeHandle handle() const { return handle_; }
  static ArkUI_NativeNodeAPI_1* api();
protected:
  virtual void OnSurfaceCreated(void* window);
  virtual void OnSurfaceChanged(void* window);
  virtual void OnSurfaceDestroyed(void* window);
  virtual void DispatchTouchEvent(void* window);
  
private:
  explicit XComponentNode(ArkUI_NodeHandle handle);
  
  static XComponentNode* GetInstance(OH_NativeXComponent* component);
  
  const ArkUI_NodeHandle handle_;
  OH_NativeXComponent* const component_;
  
  OHNativeWindow* window_ = nullptr;
  uint64_t surface_width_ = 0;
  uint64_t surface_height_ = 0;
};

}  // namespace hello

#endif //HELLOXCOMPONENT_XCOMPONENTNODE_H
