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

  renderer_thread_ = std::make_unique<Thread>(loop);
  renderer_thread_->Start();
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
  if (pending_render_pixels_count_ == 0 && window_) {
    if (type_ == kSurface) {
      DrawSurface();
    } else {
      DrawTexture();
    }
  }
}

void XComponentNode::StopDrawFrame() {
  draw_frame_ = false;
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

  OHNativeWindowBuffer* window_buffer = nullptr;
  int fenceFd = -1;
  int32_t retval = OH_NativeWindow_NativeWindowRequestBuffer(
      window_, &window_buffer, &fenceFd);
  FATAL_IF(
      retval != 0,
      "OH_NativeWindow_NativeWindowRequestBuffer() failed retval=%{public}d",
      retval);

  OH_NativeBuffer* buffer = nullptr;
  retval = OH_NativeBuffer_FromNativeWindowBuffer(window_buffer, &buffer);
  FATAL_IF(retval != 0,
           "OH_NativeBuffer_FromNativeWindowBuffer() failed retval=%{public}d",
           retval);

  if (fenceFd != -1) {
    EGLint attribs[] = {EGL_SYNC_NATIVE_FENCE_FD_ANDROID, fenceFd, EGL_NONE};
    EGLSyncKHR sync =
        gl_core->eglCreateSyncKHR(disp, EGL_SYNC_NATIVE_FENCE_ANDROID, attribs);
    FATAL_IF(sync == EGL_NO_SYNC_KHR, "eglCreateSyncKHR() failed: 0x%{public}x",
             eglGetError());

    gl_core->eglWaitSyncKHR(disp, sync, 0);
    gl_core->eglDestroySyncKHR(disp, sync);
    fenceFd = -1;

    EGLint error = eglGetError();
    FATAL_IF(error != EGL_SUCCESS, "eglGetError() returns 0x%{public}x", error);
  }

  EGLint attrs[] = {
      EGL_IMAGE_PRESERVED,
      EGL_TRUE,
      EGL_NONE,
  };
  EGLImageKHR egl_image = gl_core->eglCreateImageKHR(
      disp, EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_OHOS, window_buffer, attrs);
  FATAL_IF(egl_image == EGL_NO_IMAGE_KHR, "eglCreateImageKHR() failed!");

  GLuint texture_id = 0;
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_EXTERNAL_OES, texture_id);
  glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  gl_core->glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, egl_image);
  {
    GLenum error = glGetError();
    FATAL_IF(
        error != GL_NO_ERROR,
        "glEGLImageTargetTexture2DOES() failed with GL error: 0x%{public}x",
        error);
  }

  delegate_->RenderTexture(GL_TEXTURE_EXTERNAL_OES, texture_id, surface_width_,
                           surface_height_, 0);
  glBindTexture(GL_TEXTURE_EXTERNAL_OES, 0);
  glDeleteTextures(1, &texture_id);
  gl_core->eglDestroyImageKHR(disp, egl_image);

  {
    GLenum error = glGetError();
    FATAL_IF(
        error != GL_NO_ERROR,
        "glEGLImageTargetTexture2DOES() failed with GL error: 0x%{public}x",
        error);
  }

  {
    EGLSyncKHR sync =
        gl_core->eglCreateSyncKHR(disp, EGL_SYNC_NATIVE_FENCE_ANDROID, nullptr);
    FATAL_IF(sync == EGL_NO_SYNC, "eglCreateSyncKHR() failed");

    fenceFd = gl_core->eglDupNativeFenceFDANDROID(disp, sync);
    LOGE("EEEE fenceFd=%{public}d", fenceFd);

    gl_core->eglDestroySyncKHR(disp, sync);
    EGLint error = eglGetError();
    FATAL_IF(error != EGL_SUCCESS, "eglGetError() returns 0x%{public}x", error);
  }

  retval = OH_NativeWindow_NativeWindowFlushBuffer(window_, window_buffer,
                                                   fenceFd, {});
  FATAL_IF(retval != 0,
           "OH_NativeWindow_NativeWindowFlushBuffer() failed retval=%{public}d",
           retval);

  renderer_thread_->PostTask([] {},
                             [this] {
                               if (draw_frame_) {
                                 DrawTexture();
                               }
                             });
}

// static
XComponentNode* XComponentNode::GetInstance(OH_NativeXComponent* component) {
  auto it = xcomponent_nodes_.find(component);
  assert(it != xcomponent_nodes_.end());
  return it->second;
}

}  // namespace hello
