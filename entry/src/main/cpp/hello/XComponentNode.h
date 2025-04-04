#ifndef HELLOXCOMPONENT_XCOMPONENTNODE_H
#define HELLOXCOMPONENT_XCOMPONENTNODE_H

#include <EGL/egl.h>
#include <ace/xcomponent/native_interface_xcomponent.h>
#include <arkui/native_node.h>
#include <native_window/external_window.h>

#include <deque>
#include <memory>
#include <string>

namespace hello {

class NativeWindow;
class Renderer;
class Thread;

class XComponentNode {
 public:
  enum Type {
    // Renderer::RenderPixels() will be called with pixels address to be fixed
    // with date.
    kSoftware,
    // Renderer::RenderFrame() will be called with GL framebuffer setup.
    kEGLSurface,
    // Renderer::RenderTextre() wiil be called with target and texture_id which
    // EGLImage is bind to.
    kEGLImage,
    // Renderer::SetNativeWindow() will be called with a native window which is
    // used for rendering to.
    kNativeWindow,
    // Renderer::SetNativeWindow() will be called with a native window which is
    // used for rendering to.
    kSurfaceControl,
  };

  static std::unique_ptr<XComponentNode> Create(Renderer* renderer,
                                                const std::string& id,
                                                Type type);

  ~XComponentNode();

  void AddChild(XComponentNode* child);
  void AddChild(ArkUI_NodeHandle child);
  void StartDrawFrame();
  void StopDrawFrame();

  void SetPosition(float x, float y) { SetAttribute(NODE_POSITION, x, y); }
  void SetWidth(float width) { SetAttribute(NODE_WIDTH, width); }
  void SetHeight(float height) { SetAttribute(NODE_HEIGHT, height); }
  void SetWidthPercent(float width) { SetAttribute(NODE_WIDTH_PERCENT, width); }
  void SetHeightPercent(float height) {
    SetAttribute(NODE_HEIGHT_PERCENT, height);
  }
  void SetTranslate(float x, float y, float z) {
    SetAttribute(NODE_TRANSLATE, x, y, z);
  }
  void SetScale(float x, float y) { SetAttribute(NODE_SCALE, x, y); }
  void SetRotate(float x, float y, float z, float angle, float depth) {
    SetAttribute(NODE_ROTATE, x, y, z, angle, depth);
  }
  void SetBackgroundColor(uint32_t argb) {
    SetAttribute(NODE_BACKGROUND_COLOR, argb);
  }
  void SetFocusable(bool focusable) {
    SetAttribute(NODE_FOCUSABLE, focusable ? 1 : 0);
  }
  void SetSurfaceSize(uint32_t width, uint32_t height) {
    SetAttribute(NODE_XCOMPONENT_SURFACE_SIZE, width, height);
  }

  ArkUI_NodeHandle handle() const { return handle_; }
  static ArkUI_NativeNodeAPI_1* api();

 protected:
  virtual void OnSurfaceCreated(void* window);
  virtual void OnSurfaceChanged(void* window);
  virtual void OnSurfaceDestroyed(void* window);
  virtual void DispatchTouchEvent(void* window);
  virtual void OnFrame(uint64_t timestamp, uint64_t target_timestamp);

 private:
  XComponentNode(Renderer* renderer,
                 ArkUI_NodeHandle handle,
                 const std::string& id,
                 Type type);

  // Disable copy and move
  XComponentNode(const XComponentNode&) = delete;
  XComponentNode& operator=(const XComponentNode&) = delete;
  XComponentNode(XComponentNode&&) = delete;
  XComponentNode& operator=(XComponentNode&&) = delete;

  static XComponentNode* GetInstance(OH_NativeXComponent* component);

  void SoftwareDrawFrame();
  void HardwareDrawFrame();
  void UpdateSurfaceControl();

  void SetAttribute(ArkUI_NodeAttributeType attribute, uint32_t u32) {
    ArkUI_NumberValue value = {.u32 = u32};
    ArkUI_AttributeItem item = {&value, 1};
    api()->setAttribute(handle_, attribute, &item);
  }

  void SetAttribute(ArkUI_NodeAttributeType attribute,
                    uint32_t u1,
                    uint32_t u2) {
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

  void SetAttribute(ArkUI_NodeAttributeType attribute,
                    float f1,
                    float f2,
                    float f3) {
    ArkUI_NumberValue values[3] = {{.f32 = f1}, {.f32 = f2}, {.f32 = f3}};
    ArkUI_AttributeItem item = {values, 3};
    api()->setAttribute(handle_, attribute, &item);
  }

  void SetAttribute(ArkUI_NodeAttributeType attribute,
                    float f1,
                    float f2,
                    float f3,
                    float f4,
                    float f5) {
    ArkUI_NumberValue values[5] = {
        {.f32 = f1}, {.f32 = f2}, {.f32 = f3}, {.f32 = f4}, {.f32 = f5}};
    ArkUI_AttributeItem item = {values, 5};
    api()->setAttribute(handle_, attribute, &item);
  }

  bool is_software() const { return type_ == kSoftware; }
  bool using_egl_surface() const { return type_ == kEGLSurface; }
  bool using_egl_image() const { return type_ == kEGLImage; }
  bool using_native_window() const { return type_ == kNativeWindow; }
  bool using_surface_control() const { return type_ == kSurfaceControl; }

  void SetAttribute(ArkUI_NodeAttributeType attribute, const char* string) {
    ArkUI_AttributeItem item = {.string = string};
    api()->setAttribute(handle_, attribute, &item);
  }

  Renderer* const renderer_;
  const ArkUI_NodeHandle handle_;
  const std::string id_;
  const Type type_;
  OH_NativeXComponent* const component_;

  std::unique_ptr<Thread> renderer_thread_;

  std::unique_ptr<NativeWindow> window_;
  uint64_t surface_width_ = 0;
  uint64_t surface_height_ = 0;

  EGLSurface egl_surface_ = EGL_NO_SURFACE;

  bool draw_frame_ = false;
};

}  // namespace hello

#endif  // HELLOXCOMPONENT_XCOMPONENTNODE_H
