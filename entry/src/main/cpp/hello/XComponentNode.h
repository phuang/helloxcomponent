#ifndef HELLOXCOMPONENT_XCOMPONENTNODE_H
#define HELLOXCOMPONENT_XCOMPONENTNODE_H

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <ace/xcomponent/native_interface_xcomponent.h>
#include <arkui/native_node.h>
#include <native_window/external_window.h>

#include <deque>
#include <memory>
#include <string>

namespace hello {

class Thread;

class XComponentNode {
 public:
  class Delegate {
   public:
    virtual ~Delegate() = default;
    virtual void RenderPixels(void* pixels,
                              int32_t width,
                              int32_t height,
                              int32_t stride,
                              uint64_t timestamp) {}
    virtual void RenderTexture(GLenum target,
                               GLuint texture_id,
                               int32_t width,
                               int32_t height,
                               uint64_t timestamp) {}
    virtual void RenderFrame(int32_t width,
                             int32_t height,
                             uint64_t timestamp) {}
  };

  enum Type { kSoftware, kEGLSurface, kEGLImage };
  static std::unique_ptr<XComponentNode> Create(Delegate* delegate,
                                                const std::string& id,
                                                Type type);

  ~XComponentNode();

  void AddChild(XComponentNode* child);
  void StartDrawFrame();
  void StopDrawFrame();

  void SetPosition(float x, float y) { SetAttribute(NODE_POSITION, x, y); }
  void SetWidth(float width) { SetAttribute(NODE_WIDTH, width); }
  void SetHeight(float height) { SetAttribute(NODE_HEIGHT, height); }
  void SetWidthPercent(float width) { SetAttribute(NODE_WIDTH_PERCENT, width); }
  void SetHeightPercent(float height) {
    SetAttribute(NODE_HEIGHT_PERCENT, height);
  }
  void SetScale(float x, float y) { SetAttribute(NODE_SCALE, x, y); }
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
  XComponentNode(Delegate* delegate,
                 ArkUI_NodeHandle handle,
                 const std::string& id,
                 Type type);

  static XComponentNode* GetInstance(OH_NativeXComponent* component);

  void SoftwareDrawFrame();
  void HardwareDrawFrame();

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

  bool is_software() const { return type_ == kSoftware; }
  bool using_egl_surface() const { return type_ == kEGLSurface; }
  bool using_egl_image() const { return type_ == kEGLImage; }

  void SetAttribute(ArkUI_NodeAttributeType attribute, const char* string) {
    ArkUI_AttributeItem item = {.string = string};
    api()->setAttribute(handle_, attribute, &item);
  }

  Delegate* const delegate_;
  const ArkUI_NodeHandle handle_;
  const std::string id_;
  const Type type_;
  OH_NativeXComponent* const component_;

  std::unique_ptr<Thread> renderer_thread_;

  OHNativeWindow* window_ = nullptr;
  uint64_t surface_width_ = 0;
  uint64_t surface_height_ = 0;

  EGLSurface egl_surface_ = EGL_NO_SURFACE;

  int pending_render_pixels_count_ = 0;
  bool draw_frame_ = false;
};

}  // namespace hello

#endif  // HELLOXCOMPONENT_XCOMPONENTNODE_H
