#include "hello/NapiManager.h"

#include <arkui/native_node.h>
#include <arkui/native_node_napi.h>

#include <cassert>
#include <cstdint>
#include <string>

#include "common/log.h"
#include "hello/DelegatedNodeContent.h"

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
}

NapiManager::~NapiManager() = default;

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

  if (!info[1].IsBoolean()) {
    Napi::Error::New(env, "Arg 1 is not a boolean")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  bool delegated = info[1].As<Napi::Boolean>().Value();
  GetInstance()->CreateNativeNode(content_handle, delegated);

  return env.Null();
}

// static
Napi::Value NapiManager::NapiSetDelegatedCompositing(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() != 1) {
    Napi::Error::New(env, "Wrong number of arguments")
        .ThrowAsJavaScriptException();
    return env.Null();
  }

  if (!info[0].IsBoolean()) {
    Napi::Error::New(env, "Arg 0 is not a boolean")
        .ThrowAsJavaScriptException();
    return env.Null();
  }

  bool enable = info[0].As<Napi::Boolean>().Value();

  GetInstance()->SetDelegatedCompositing(enable);

  return env.Null();
}

void NapiManager::CreateNativeNode(ArkUI_NodeContentHandle content_handle,
                                   bool delegated) {
  if (delegated) {
    delegated_node_content_ =
        std::make_unique<hello::DelegatedNodeContent>(content_handle);
  } else {
  }
}

void NapiManager::SetDelegatedCompositing(bool enable) {
  if (enable) {
    delegated_node_content_->SetVisible(true);
  } else {
    delegated_node_content_->SetVisible(false);
  }
}

}  // namespace hello
