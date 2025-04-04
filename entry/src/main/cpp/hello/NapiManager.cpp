#include "hello/NapiManager.h"

#include <arkui/native_node.h>
#include <arkui/native_node_napi.h>

#include <cassert>
#include <cstdint>
#include <string>

#include "hello/DelegatedNodeContent.h"
#include "hello/DisplayManager.h"
#include "hello/GLCore.h"
#include "hello/Log.h"
#include "hello/NonDelegatedNodeContent.h"
#include "hello/SurfaceControlNodeContent.h"

// #include "surface_control/ng/surface_control_model.h"

namespace hello {
namespace {
NapiManager* napi_manager_ = nullptr;
}

// static
void NapiManager::Init(const Napi::Env& env) {
  CHECK(!napi_manager_);
  napi_manager_ = new NapiManager(env);
}

// static
NapiManager* NapiManager::GetInstance() {
  CHECK(napi_manager_);
  return napi_manager_;
}

NapiManager::NapiManager(const Napi::Env& env) : env_(env) {
  std::srand(std::time({}));
  gl_core_ = std::make_unique<GLCore>();
  gl_core_->Init();
  display_manager_ = std::make_unique<DisplayManager>();
}

NapiManager::~NapiManager() {
  gl_core_->Destroy();
}

// static
Napi::Value NapiManager::NapiCreateNativeNode(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() != 2) {
    Napi::Error::New(env, "Wrong number of arguments")
        .ThrowAsJavaScriptException();
    return env.Null();
  }

  ArkUI_NodeContentHandle content_handle = nullptr;
  int32_t retval =
      OH_ArkUI_GetNodeContentFromNapiValue(env, info[0], &content_handle);
  if (retval != ARKUI_ERROR_CODE_NO_ERROR) {
    Napi::Error::New(env, "Arg 0 is not a NodeContent")
        .ThrowAsJavaScriptException();
    return env.Null();
  }

  if (!info[1].IsNumber()) {
    Napi::Error::New(env, "Arg 1 is not a number").ThrowAsJavaScriptException();
    return env.Null();
  }
  int mode = info[1].As<Napi::Number>().Int32Value();
  GetInstance()->CreateNativeNode(content_handle, mode);

  return env.Null();
}

// static
Napi::Value NapiManager::NapiSetCurrentMode(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() != 1) {
    Napi::Error::New(env, "Wrong number of arguments")
        .ThrowAsJavaScriptException();
    return env.Null();
  }

  if (!info[0].IsNumber()) {
    Napi::Error::New(env, "Arg 0 is not a number").ThrowAsJavaScriptException();
    return env.Null();
  }

  int mode = info[0].As<Napi::Number>().Int32Value();

  GetInstance()->SetCurrentMode(mode);

  return env.Null();
}

// static
Napi::Value NapiManager::NapiOnPageShow(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  GetInstance()->OnPageShow();
  return env.Null();
}

// static
Napi::Value NapiManager::NapiOnPageHide(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  GetInstance()->OnPageHide();
  return env.Null();
}

void NapiManager::CreateNativeNode(ArkUI_NodeContentHandle content_handle,
                                   int mode) {
  if (mode == 0) {
    delegated_node_content_ =
        std::make_unique<DelegatedNodeContent>(content_handle);
  } else if (mode == 1) {
    non_delegated_node_content_ =
        std::make_unique<NonDelegatedNodeContent>(content_handle);
  } else if (mode == 2) {
    surface_control_node_content_ =
        std::make_unique<SurfaceControlNodeContent>(content_handle);
  }
}

void NapiManager::SetCurrentMode(int mode) {
  current_mode_ = mode;
  Update();
}

void NapiManager::OnPageShow() {
  is_visible_ = true;
  Update();
}

void NapiManager::OnPageHide() {
  is_visible_ = false;
  Update();
}

void NapiManager::Update() {
  delegated_node_content_->SetVisible(is_visible_ && current_mode_ == 0);
  non_delegated_node_content_->SetVisible(is_visible_ && current_mode_ == 1);
  surface_control_node_content_->SetVisible(is_visible_ && current_mode_ == 2);
}

}  // namespace hello
