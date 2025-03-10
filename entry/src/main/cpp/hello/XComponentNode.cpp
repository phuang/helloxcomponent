#include "hello/XComponentNode.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <arkui/native_interface.h>
#include <native_buffer/native_buffer.h>
#include <native_window/external_window.h>
#include <unistd.h>
#include <uv.h>

#include <cassert>
#include <functional>
#include <map>
#include <mutex>

#include "common/log.h"
#include "hello/BitmapRenderer.h"
#include "hello/GLCore.h"
#include "hello/GLFence.h"
#include "hello/GLImage.h"
#include "hello/NapiManager.h"
#include "hello/Thread.h"

namespace hello {
namespace {

std::map<OH_NativeXComponent*, XComponentNode*> xcomponent_nodes_;
}  // namespace

// static
std::unique_ptr<XComponentNode> XComponentNode::Create(Delegate* delegate,
                                                       std::string id,
                                                       Type type) {
  ArkUI_NodeHandle handle = api()->createNode(ARKUI_NODE_XCOMPONENT);
  if (!handle) {
    return {};
  }

  std::unique_ptr<XComponentNode> component(
      new XComponentNode(delegate, handle, type));

  component->SetAttribute(NODE_XCOMPONENT_ID, id.c_str());
  component->SetAttribute(NODE_XCOMPONENT_TYPE,
                          type == kSurface ? ARKUI_XCOMPONENT_TYPE_SURFACE
                                           : ARKUI_XCOMPONENT_TYPE_TEXTURE);

  return component;
}

XComponentNode::XComponentNode(Delegate* delegate,
                               ArkUI_NodeHandle handle,
                               Type type)
    : delegate_(delegate),
      handle_(handle),
      type_(type),
      component_(OH_NativeXComponent_GetNativeXComponent(handle_)) {
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
  FATAL_IF(retval != 0,
           "OH_NativeXComponent_RegisterCallback() failed retval=%{public}d",
           retval);

  const auto& env = NapiManager::GetInstance()->env();
  uv_loop_t* loop = nullptr;
  napi_status status = napi_get_uv_event_loop(env, &loop);
  FATAL_IF(status != napi_ok,
           "napi_get_uv_event_loop() failed status=%{public}d", status);

  if (is_surface()) {
    // If is surface, the xcomponent is rastered with CPU on a separated CPU
    renderer_thread_ = std::make_unique<Thread>(loop);
    renderer_thread_->Start();
  }
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
    api = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE,
                                            "ArkUI_NativeNodeAPI_1"));
  });
  return api;
}

void XComponentNode::AddChild(XComponentNode* child) {
  api()->addChild(handle(), child->handle());
}

void XComponentNode::StartDrawFrame() {
  if (!delegate_) {
    return;
  }

  if (draw_frame_) {
    return;
  }
  draw_frame_ = true;

  switch (type_) {
    case kSurface: {
      if (pending_render_pixels_count_ > 0) {
        // There is pending RenderPixels() calls, when the pending calls are
        // done, new frame will be initialed automactilly.
        break;
      }
      if (!window_) {
        // Surface is not created yet, delay drawing frame until surface is
        // created.
        break;
      }
      DrawSurface();
      break;
    }
    case kTexture: {
      OH_NativeXComponent_RegisterOnFrameCallback(
          component_, [](OH_NativeXComponent* component, uint64_t timestamp,
                         uint64_t target_timestamp) {
            GetInstance(component)->OnFrame(timestamp, target_timestamp);
          });
      break;
    }
  }
}

void XComponentNode::StopDrawFrame() {
  draw_frame_ = false;
  switch (type_) {
    case kSurface: {
      // Do noting, when pending RenderPixels are done, not new frame will be
      // draw.
      break;
    }
    case kTexture: {
      // Unregister the on frame callback to stop new frames.
      OH_NativeXComponent_UnregisterOnFrameCallback(component_);
      break;
    }
  }
}

void XComponentNode::OnSurfaceCreated(void* window) {
  LOGE("XComponentNode::%{public}s()", __func__);
  window_ = reinterpret_cast<OHNativeWindow*>(window);

  int32_t retval = OH_NativeXComponent_GetXComponentSize(
      component_, window_, &surface_width_, &surface_height_);
  FATAL_IF(retval != 0,
           "OH_NativeXComponent_GetXComponentSize() failed retval=%{public}d",
           retval);

  if (draw_frame_) {
    CHECK(delegate_);
    if (type_ == kSurface) {
      DrawSurface();
    } else {
      DrawTexture();
    }
  }
  // LOGE(
  //     "EEEE OH_NativeXComponent_GetXComponentSize() return %{public}d "
  //     "%{public}lux%{public}lu",
  //     retval, surface_width_, surface_height_);
  //
  // OH_NativeXComponent_ExpectedRateRange range;
  // retval = OH_NativeXComponent_SetExpectedFrameRateRange(component_,
  // nullptr); FATAL_IF(retval != 0,
  //          "OH_NativeXComponent_SetExpectedFrameRateRange() failed
  //          retval=%{public}d", retval);
  //
  // retval = OH_NativeXComponent_RegisterOnFrameCallback(
  //     component_, [](OH_NativeXComponent* component, uint64_t timestamp,
  //                    uint64_t target_timestamp) {
  //       GetInstance(component)->OnFrame(timestamp, target_timestamp);
  //     });
  // FATAL_IF(
  //     retval != 0,
  //     "OH_NativeXComponent_RegisterOnFrameCallback() failed
  //     retval=%{public}d", retval);
}

void XComponentNode::OnSurfaceChanged(void* window) {}

void XComponentNode::OnSurfaceDestroyed(void* window) {
  // OH_NativeXComponent_UnregisterOnFrameCallback(component_);
}

void XComponentNode::DispatchTouchEvent(void* window) {}

struct RenderPixelsData {
  XComponentNode* self;
  void* pixels;
  int32_t width;
  int32_t height;
  int32_t stride;
  uint64_t timestamp;
  OHNativeWindowBuffer* window_buffer;
  OH_NativeBuffer* buffer;
};

void XComponentNode::OnFrame(uint64_t timestamp, uint64_t targetTimestamp) {
  CHECK(type_ == kTexture);
  if (window_) {
    DrawTexture();
  }
}

void XComponentNode::DrawSurface() {
  OHNativeWindowBuffer* window_buffer = nullptr;
  int fenceFd = -1;
  int32_t retval = OH_NativeWindow_NativeWindowRequestBuffer(
      window_, &window_buffer, &fenceFd);
  FATAL_IF(
      retval != 0,
      "OH_NativeWindow_NativeWindowRequestBuffer() failed retval=%{public}d",
      retval);

  if (fenceFd != -1) {
    close(fenceFd);
  }

  OH_NativeBuffer* buffer = nullptr;
  retval = OH_NativeBuffer_FromNativeWindowBuffer(window_buffer, &buffer);
  FATAL_IF(retval != 0,
           "OH_NativeBuffer_FromNativeWindowBuffer() failed retval=%{public}d",
           retval);

  OH_NativeBuffer_Config config;
  OH_NativeBuffer_GetConfig(buffer, &config);

  void* addr = nullptr;
  retval = OH_NativeBuffer_Map(buffer, &addr);
  FATAL_IF(retval != 0, "OH_NativeBuffer_Map() failed retval=%{public}d",
           retval);

  ++pending_render_pixels_count_;
  renderer_thread_->PostTask(
      [this, addr, config, window_buffer] {
        delegate_->RenderPixels(addr, config.width, config.height,
                                config.stride, 0);
      },
      [this, window_buffer, buffer] {
        OH_NativeBuffer_Unmap(buffer);
        int32_t retval = OH_NativeWindow_NativeWindowFlushBuffer(
            window_, window_buffer, -1, {});
        FATAL_IF(retval != 0,
                 "OH_NativeWindow_NativeWindowFlushBuffer() failed "
                 "retval=%{public}d",
                 retval);
        if (draw_frame_) {
          DrawSurface();
        }
        --pending_render_pixels_count_;
      });
}

void XComponentNode::DrawTexture() {
  auto disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  const auto* gl_core = NapiManager::GetInstance()->gl_core();

  CHECK(window_);

  OHNativeWindowBuffer* window_buffer = nullptr;
  int fd = -1;
  int32_t retval = OH_NativeWindow_NativeWindowRequestBuffer(
      window_, &window_buffer, &fd);
  FATAL_IF(
      retval != 0,
      "OH_NativeWindow_NativeWindowRequestBuffer() failed retval=%{public}d",
      retval);

  ScopedFd fence_fd(fd);

  if (fence_fd.is_valid()) {
    auto fence = GLFence::CreateFromFenceFd(std::move(fence_fd));
    fence->Wait();

    EGLint error = eglGetError();
    FATAL_IF(error != EGL_SUCCESS, "eglGetError() returns 0x%{public}x", error);
  }

  {
    GLImage image;
    image.Initialize(window_buffer);

    GLTexture texture = image.Bind();

    {
      GLenum error = glGetError();
      FATAL_IF(
          error != GL_NO_ERROR,
          "glEGLImageTargetTexture2DOES() failed with GL error: 0x%{public}x",
          error);
    }

    delegate_->RenderTexture(texture.target(), texture.texture(),
                             surface_width_, surface_height_, 0);
  }

  {
    auto fence = GLFence::Create();
    fence_fd = fence->GetFd();
    EGLint error = eglGetError();
    FATAL_IF(error != EGL_SUCCESS, "eglGetError() returns 0x%{public}x", error);
  }

  retval = OH_NativeWindow_NativeWindowFlushBuffer(window_, window_buffer,
                                                   fence_fd.release(), {});
  FATAL_IF(retval != 0,
           "OH_NativeWindow_NativeWindowFlushBuffer() failed retval=%{public}d",
           retval);
}

// static
XComponentNode* XComponentNode::GetInstance(OH_NativeXComponent* component) {
  auto it = xcomponent_nodes_.find(component);
  assert(it != xcomponent_nodes_.end());
  return it->second;
}

}  // namespace hello
