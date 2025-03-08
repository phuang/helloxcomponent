#ifndef HELLOXCOMPONENT_XCOMPONENTNODE_H
#define HELLOXCOMPONENT_XCOMPONENTNODE_H

#include "hello/BitmapRenderer.h"
#include <ace/xcomponent/native_interface_xcomponent.h>
#include <arkui/native_node.h>
#include <native_window/external_window.h>

#include <memory>
#include <string>

namespace hello {

class XComponentNode {
public:
  enum Type { kSurface, kTexture };

  static std::unique_ptr<XComponentNode> Create(std::string id, Type type);
  ~XComponentNode();

  void SetPosition(float x, float y) { SetAttribute(NODE_POSITION, x, y); }
  void SetWidth(float width) { SetAttribute(NODE_WIDTH, width); }
  void SetHeight(float height) { SetAttribute(NODE_HEIGHT, height); }
  void SetWidthPercent(float width) { SetAttribute(NODE_WIDTH_PERCENT, width); }
  void SetHeightPercent(float height) { SetAttribute(NODE_HEIGHT_PERCENT, height); }
  void SetScale(float x, float y) { SetAttribute(NODE_SCALE, x, y); }
  void SetBackgroundColor(uint32_t argb) { SetAttribute(NODE_BACKGROUND_COLOR, argb); }
  void SetFocusable(bool focusable) { SetAttribute(NODE_FOCUSABLE, focusable ? 1 : 0); }
  void SetSurfaceSize(uint32_t width, uint32_t height) { SetAttribute(NODE_XCOMPONENT_SURFACE_SIZE, width, height); }

  void AddChild(XComponentNode *child);

  ArkUI_NodeHandle handle() const { return handle_; }
  static ArkUI_NativeNodeAPI_1 *api();

protected:
  virtual void OnSurfaceCreated(void *window);
  virtual void OnSurfaceChanged(void *window);
  virtual void OnSurfaceDestroyed(void *window);
  virtual void DispatchTouchEvent(void *window);
  virtual void OnFrame(uint64_t timestamp, uint64_t target_timestamp);

private:
  explicit XComponentNode(ArkUI_NodeHandle handle);

  static XComponentNode *GetInstance(OH_NativeXComponent *component);

  void SetAttribute(ArkUI_NodeAttributeType attribute, uint32_t u32) {
    ArkUI_NumberValue value = {.u32 = u32};
    ArkUI_AttributeItem item = {&value, 1};
    api()->setAttribute(handle_, attribute, &item);
  }

  void SetAttribute(ArkUI_NodeAttributeType attribute, uint32_t u1, uint32_t u2) {
    ArkUI_NumberValue values[2] = {{.u32 = u1}, {.u32 = u2}};
    ArkUI_AttributeItem item = {values, 2};
    api()->setAttribute(handle_, attribute, &item);
  }

  void SetAttribute(ArkUI_NodeAttributeType attribute, int32_t u32) {
    ArkUI_NumberValue value = {.i32 = u32};
    ArkUI_AttributeItem item = {&value, 1};
    api()->setAttribute(handle_, attribute, &item);
  }

  void SetAttribute(ArkUI_NodeAttributeType attribute, float f32) {
    ArkUI_NumberValue value = {.f32 = f32};
    ArkUI_AttributeItem item = {&value, 1};
    api()->setAttribute(handle_, attribute, &item);
  }

  void SetAttribute(ArkUI_NodeAttributeType attribute, float f1, float f2) {
    ArkUI_NumberValue values[2] = {{.f32 = f1}, {.f32 = f2}};
    ArkUI_AttributeItem item = {values, 2};
    api()->setAttribute(handle_, attribute, &item);
  }

  void SetAttribute(ArkUI_NodeAttributeType attribute, const char *string) {
    ArkUI_AttributeItem item = {.string = string};
    api()->setAttribute(handle_, attribute, &item);
  }

  const ArkUI_NodeHandle handle_;
  OH_NativeXComponent *const component_;

  OHNativeWindow *window_ = nullptr;
  uint64_t surface_width_ = 0;
  uint64_t surface_height_ = 0;
  
  std::unique_ptr<BitmapRenderer> bitmap_renderer_;
};

} // namespace hello

#endif // HELLOXCOMPONENT_XCOMPONENTNODE_H
