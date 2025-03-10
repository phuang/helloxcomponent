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

#include "hello/Log.h"
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
                                                       const std::string& id,
                                                       Type type) {
  ArkUI_NodeHandle handle = api()->createNode(ARKUI_NODE_XCOMPONENT);
  FATAL_IF(handle == nullptr, "createNode(ARKUI_NODE_XCOMPONENT) failed!");

  std::unique_ptr<XComponentNode> component(
      new XComponentNode(delegate, handle, id, type));

  return component;
}

XComponentNode::XComponentNode(Delegate* delegate,
                               ArkUI_NodeHandle handle,
                               const std::string& id,
                               Type type)
    : delegate_(delegate),
      handle_(handle),
      id_(id),
      type_(type),
      component_(OH_NativeXComponent_GetNativeXComponent(handle_)) {
  assert(component_);
  xcomponent_nodes_[component_] = this;

  SetAttribute(NODE_XCOMPONENT_ID, id_.c_str());
  SetAttribute(NODE_XCOMPONENT_TYPE, ARKUI_XCOMPONENT_TYPE_SURFACE);

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

  if (!delegate_) {
    // If delegate_ is null, no need to draw.
    return;
  }

  if (is_software()) {
    // If is surface with delegate_ for rendering, the xcomponent is rastered
    // with CPU on a separated thread.
    const auto& env = NapiManager::GetInstance()->env();
    uv_loop_t* loop = nullptr;
    napi_status status = napi_get_uv_event_loop(env, &loop);
    FATAL_IF(status != napi_ok,
             "napi_get_uv_event_loop() failed status=%{public}d", status);

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
    // If delegate_ is nullptr, no need to draw.
    return;
  }

  if (draw_frame_) {
    return;
  }
  draw_frame_ = true;

  if (is_software()) {
    if (pending_render_pixels_count_ > 0) {
      // There is pending RenderPixels() calls, when the pending calls are
      // done, new frame will be initialed automactilly.
      return;
    }
    if (!window_) {
      // Surface is not created yet, delay drawing frame until surface is
      // created.
      return;
    }
    SoftwareDrawFrame();
    return;
  }

  OH_NativeXComponent_RegisterOnFrameCallback(
      component_, [](OH_NativeXComponent* component, uint64_t timestamp,
                     uint64_t target_timestamp) {
        GetInstance(component)->OnFrame(timestamp, target_timestamp);
      });
}

void XComponentNode::StopDrawFrame() {
  if (!delegate_) {
    CHECK(!draw_frame_);
    return;
  }

  draw_frame_ = false;
  if (is_software()) {
    // Do noting, when pending RenderPixels are done, not new frame will be
    // draw.
    return;
  }

  OH_NativeXComponent_UnregisterOnFrameCallback(component_);
}

void XComponentNode::OnSurfaceCreated(void* window) {
  OnSurfaceChanged(window);

  if (draw_frame_) {
    CHECK(delegate_);
    if (is_software()) {
      SoftwareDrawFrame();
    } else {
      HardwareDrawFrame();
    }
  }
}

void XComponentNode::OnSurfaceChanged(void* window) {
  window_ = reinterpret_cast<OHNativeWindow*>(window);
  int32_t retval = OH_NativeXComponent_GetXComponentSize(
      component_, window_, &surface_width_, &surface_height_);
  FATAL_IF(retval != 0,
           "OH_NativeXComponent_GetXComponentSize() failed retval=%{public}d",
           retval);
  if (using_egl_surface()) {
    const auto* gl_core = NapiManager::GetInstance()->gl_core();
    EGLDisplay display = gl_core->display();
    egl_surface_ =
        eglCreateWindowSurface(display, gl_core->config(), window_, nullptr);
    FATAL_IF(egl_surface_ == EGL_NO_SURFACE,
             "eglCreateWindowSurface() failed. EGL error: 0x%{public}x",
             eglGetError());
  }
}

void XComponentNode::OnSurfaceDestroyed(void* window) {}

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
  CHECK(!is_software());
  if (window_) {
    HardwareDrawFrame();
  }
}

void XComponentNode::SoftwareDrawFrame() {
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
          SoftwareDrawFrame();
        }
        --pending_render_pixels_count_;
      });
}

void XComponentNode::HardwareDrawFrame() {
  CHECK(window_);

  if (using_egl_surface()) {
    CHECK(egl_surface_ != EGL_NO_SURFACE);
    const auto* gl_core = NapiManager::GetInstance()->gl_core();
    EGLDisplay display = gl_core->display();
    EGLContext context = gl_core->context();
    if (!eglMakeCurrent(display, egl_surface_, egl_surface_, context)) {
      FATAL("eglMakeCurrent() failed. EGL error: 0x%{public}x", eglGetError());
    }
    delegate_->RenderFrame(surface_width_, surface_height_, 0);
    eglSwapBuffers(display, egl_surface_);
    return;
  }

  OHNativeWindowBuffer* window_buffer = nullptr;
  int fd = -1;
  int32_t retval =
      OH_NativeWindow_NativeWindowRequestBuffer(window_, &window_buffer, &fd);
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
    if (!image.Initialize(window_buffer)) {
      FATAL("Initialize EGLImageKHR failed!");
    }

    GLTexture texture = image.Bind();
    delegate_->RenderTexture(texture.target(), texture.texture(),
                             surface_width_, surface_height_, 0);
  }

  {
    auto fence = GLFence::Create();
    if (!fence) {
      FATAL("Create EGLSyncKHR failed!");
    }
    fence_fd = fence->GetFd();
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
