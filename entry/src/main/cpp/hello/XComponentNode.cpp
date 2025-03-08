#include "hello/XComponentNode.h"

#include <arkui/native_interface.h>
#include <native_buffer/native_buffer.h>
#include <native_window/external_window.h>
#include <unistd.h>

#include <cassert>
#include <map>
#include <mutex>

#include "common/log.h"
#include "hello/BitmapRenderer.h"

namespace hello {
namespace {
std::map<OH_NativeXComponent *, XComponentNode *> xcomponent_nodes_;
}

// static
std::unique_ptr<XComponentNode> XComponentNode::Create(std::string id, Type type) {
  ArkUI_NodeHandle handle = api()->createNode(ARKUI_NODE_XCOMPONENT);
  if (!handle) {
    return {};
  }

  std::unique_ptr<XComponentNode> component(new XComponentNode(handle));

  component->SetAttribute(NODE_XCOMPONENT_TYPE,
                          type == kSurface ? ARKUI_XCOMPONENT_TYPE_SURFACE : ARKUI_XCOMPONENT_TYPE_TEXTURE);
  component->SetAttribute(NODE_XCOMPONENT_ID, id.c_str());

  return component;
}

XComponentNode::XComponentNode(ArkUI_NodeHandle handle)
    : handle_(handle), component_(OH_NativeXComponent_GetNativeXComponent(handle_)),
      bitmap_renderer_(std::make_unique<BitmapRenderer>()) {
  assert(component_);
  xcomponent_nodes_[component_] = this;
  static OH_NativeXComponent_Callback callbacks = {
      [](OH_NativeXComponent *component, void *window) { GetInstance(component)->OnSurfaceCreated(window); },
      [](OH_NativeXComponent *component, void *window) { GetInstance(component)->OnSurfaceChanged(window); },
      [](OH_NativeXComponent *component, void *window) { GetInstance(component)->OnSurfaceDestroyed(window); },
      [](OH_NativeXComponent *component, void *window) { GetInstance(component)->DispatchTouchEvent(window); },
  };
  int32_t retval = OH_NativeXComponent_RegisterCallback(component_, &callbacks);
  LOGE("EEEE OH_NativeXComponent_RegisterCallback() return %{public}d", retval);
}

XComponentNode::~XComponentNode() {
  xcomponent_nodes_.erase(component_);
  api()->disposeNode(handle_);
}

// static
ArkUI_NativeNodeAPI_1 *XComponentNode::api() {
  static ArkUI_NativeNodeAPI_1 *api = nullptr;
  static std::once_flag flag;
  std::call_once(flag, [&]() {
    api = reinterpret_cast<ArkUI_NativeNodeAPI_1 *>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
  });
  return api;
}

void XComponentNode::AddChild(XComponentNode *child) { api()->addChild(handle(), child->handle()); }


void XComponentNode::OnSurfaceCreated(void *window) {
  LOGE("XComponentNode::%{public}s()", __func__);
  window_ = reinterpret_cast<OHNativeWindow *>(window);

  int32_t retval = OH_NativeXComponent_GetXComponentSize(component_, window_, &surface_width_, &surface_height_);
  LOGE("EEEE OH_NativeXComponent_GetXComponentSize() return %{public}d %{public}lux%{public}lu", retval, surface_width_,
       surface_height_);

  // OH_NativeXComponent_ExpectedRateRange range;
  retval = OH_NativeXComponent_SetExpectedFrameRateRange(component_, nullptr);
  LOGE("EEEE OH_NativeXComponent_SetExpectedFrameRateRange() return %{public}d", retval);

  retval = OH_NativeXComponent_RegisterOnFrameCallback(
      component_, [](OH_NativeXComponent *component, uint64_t timestamp, uint64_t target_timestamp) {
        GetInstance(component)->OnFrame(timestamp, target_timestamp);
      });
}

void XComponentNode::OnSurfaceChanged(void *window) { LOGE("XComponentNode::%{public}s()", __func__); }

void XComponentNode::OnSurfaceDestroyed(void *window) {
  LOGE("XComponentNode::%{public}s()", __func__);
  OH_NativeXComponent_UnregisterOnFrameCallback(component_);
}

void XComponentNode::DispatchTouchEvent(void *window) { LOGE("XComponentNode::%{public}s()", __func__); }

void XComponentNode::OnFrame(uint64_t timestamp, uint64_t target_timestamp) {
//  LOGE("XComponentNode::%{public}s()", __func__);
  OHNativeWindowBuffer *window_buffer = nullptr;
  int fenceFd = -1;
  int32_t retval = OH_NativeWindow_NativeWindowRequestBuffer(window_, &window_buffer, &fenceFd);
  if (fenceFd != -1) {
    close(fenceFd);
  }
//  LOGE("EEEE OH_NativeWindow_NativeWindowRequestBuffer() return %{public}d fenceFd=%{public}d", retval, fenceFd);

  OH_NativeBuffer *buffer = nullptr;
  retval = OH_NativeBuffer_FromNativeWindowBuffer(window_buffer, &buffer);

  OH_NativeBuffer_Config config;
  OH_NativeBuffer_GetConfig(buffer, &config);

//  LOGE("EEEE NativeBuffer: size=%{public}dx%{public}d stride=%{public}d format=%{public}d", config.width,
  //  config.height,
//       config.stride, config.format);


  void *addr = nullptr;
  retval = OH_NativeBuffer_Map(buffer, &addr);

  bitmap_renderer_->Render(addr, config.width, config.height, config.stride, target_timestamp);

  //  LOGE("EEEE OH_NativeBuffer_Map() target_timestamp=%{public}lu", target_timestamp);
//
//  LOGE("EEEE OnFrame() return %{public}d", retval);

//  static uint32_t frame = 0;
//  frame = (++frame % 256);
//
//  uint32_t *line = reinterpret_cast<uint32_t *>(addr);
//  for (uint64_t y = 0; y < config.height; ++y) {
//    auto *pixel = line;
//    for (uint64_t x = 0; x < config.width; ++x) {
//      *pixel = (((x * 256 / config.width) + frame) & 0xff) | ((((y * 256 / config.height) + frame) & 0xff) << 8);
//      pixel++;
//    }
//    line += config.stride / 4;
//  }

  OH_NativeBuffer_Unmap(buffer);
  retval = OH_NativeWindow_NativeWindowFlushBuffer(window_, window_buffer, -1, {});
}

// static
XComponentNode *XComponentNode::GetInstance(OH_NativeXComponent *component) {
  auto it = xcomponent_nodes_.find(component);
  assert(it != xcomponent_nodes_.end());
  return it->second;
}

} // namespace hello