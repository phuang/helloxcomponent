/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "napi_manager.h"

#include <ace/xcomponent/native_interface_xcomponent.h>
#include <arkui/native_node.h>
#include <arkui/native_interface.h>

#include <dlfcn.h>

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

#include "common/log.h"
#include "hello/XComponentNode.h"

namespace helloxcomponent {

enum class ContextType {
  APP_LIFECYCLE = 0,
  JS_PAGE_LIFECYCLE,
};

// static
NapiManager* NapiManager::GetInstance() {
  static NapiManager manager;
  return &manager;
}

NapiManager::NapiManager() = default;

NapiManager::~NapiManager() = default;

// static
Napi::Value NapiManager::GetContext(const Napi::CallbackInfo& info) {
  LOGE("NapiManager::GetContext()");
  Napi::Env env = info.Env();

  if (info.Length() != 1) {
    Napi::Error::New(env, "Wrong number of arguments")
        .ThrowAsJavaScriptException();
    return env.Null();
  }

  if (!info[0].IsNumber()) {
    Napi::Error::New(env, "Wrong arguments").ThrowAsJavaScriptException();
    return env.Null();
  }

  int64_t value = info[0].As<Napi::Number>().Int64Value();

  auto exports = Napi::Object::New(env);

  switch (static_cast<ContextType>(value)) {
    case ContextType::APP_LIFECYCLE: {
      /* AppInit对应EntryAbility.ts中的应用生命周期: onCreate, onShow, onHide,
       * onDestroy */
      LOGE("NapiManager::GetContext() APP_LIFECYCLE");

      exports.Set("onCreate",
                  Napi::Function::New<NapiManager::NapiOnCreate>(env));
      exports.Set("onShow", Napi::Function::New<NapiManager::NapiOnShow>(env));
      exports.Set("onHide", Napi::Function::New<NapiManager::NapiOnHide>(env));
      exports.Set("onDestroy",
                  Napi::Function::New<NapiManager::NapiOnDestroy>(env));
    }

    break;
    case ContextType::JS_PAGE_LIFECYCLE: {
      /* JS Page */
      LOGE("NapiManager::GetContext() JS_PAGE_LIFECYCLE");

      exports.Set("onPageShow",
                  Napi::Function::New<NapiManager::NapiOnPageShow>(env));
      exports.Set("onPageHide",
                  Napi::Function::New<NapiManager::NapiOnPageHide>(env));
    } break;
    default: {
      LOGE("unknown type");
      break;
    }
  }
  return exports;
}

bool NapiManager::Export(Napi::Env env, Napi::Object exports) {
  LOGE("NapiManager::Export()");
  if (env_ == nullptr) {
    env_ = env;
  }

  if (!exports.Has(OH_NATIVE_XCOMPONENT_OBJ)) {
    LOGE("NapiManager::Export() cannot get xcomponent obj");
    return false;
  }

  auto export_instance =
      exports.Get(OH_NATIVE_XCOMPONENT_OBJ).As<Napi::Object>();
  auto* native_xcomponent =
      Napi::ObjectWrap<OH_NativeXComponent>::Unwrap(export_instance);
  if (!native_xcomponent) {
    LOGE("NapiManager::Export() cannot unwrap xcomponent obj");
    return false;
  }

  char buf[OH_XCOMPONENT_ID_LEN_MAX + 1] = {};
  uint64_t size = OH_XCOMPONENT_ID_LEN_MAX + 1;

  int32_t ret =
      OH_NativeXComponent_GetXComponentId(native_xcomponent, buf, &size);
  if (ret != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
    LOGE("NapiManager::Export() cannot get xcomponent id");
    return false;
  }
  
  SetRootViewXComp(native_xcomponent);
  
  auto node = XComponentNode::Create("root_view", XComponentNode::kSurface);
  
  return true;
}

void NapiManager::OnCreateNative(Napi::Env env) {
  LOGE("NapiManager::%{public}s()", __func__);
}

void NapiManager::OnShowNative() {
  LOGE("NapiManager::%{public}s()", __func__);
}

void NapiManager::OnHideNative() {
  LOGE("NapiManager::%{public}s()", __func__);
}

void NapiManager::OnDestroyNative() {
  LOGE("NapiManager::%{public}s()", __func__);
}

void NapiManager::OnPageShowNative() {
  LOGE("NapiManager::%{public}s()", __func__);
}

void NapiManager::OnPageHideNative() {
  LOGE("NapiManager::%{public}s()", __func__);
}

// static
Napi::Value NapiManager::NapiOnCreate(const Napi::CallbackInfo& info) {
  auto env = info.Env();
  NapiManager::GetInstance()->OnCreateNative(env);
  return env.Null();
}

// static
Napi::Value NapiManager::NapiOnShow(const Napi::CallbackInfo& info) {
  auto env = info.Env();
  NapiManager::GetInstance()->OnShowNative();
  return env.Null();
}

// static
Napi::Value NapiManager::NapiOnHide(const Napi::CallbackInfo& info) {
  auto env = info.Env();
  NapiManager::GetInstance()->OnHideNative();
  return env.Null();
}

// static
Napi::Value NapiManager::NapiOnDestroy(const Napi::CallbackInfo& info) {
  auto env = info.Env();
  NapiManager::GetInstance()->OnDestroyNative();
  return env.Null();
}

// static
Napi::Value NapiManager::NapiOnPageShow(const Napi::CallbackInfo& info) {
  auto env = info.Env();
  NapiManager::GetInstance()->OnPageShowNative();
  return env.Null();
}

// static
Napi::Value NapiManager::NapiOnPageHide(const Napi::CallbackInfo& info) {
  auto env = info.Env();
  NapiManager::GetInstance()->OnPageHideNative();
  return env.Null();
}
}  // namespace helloxcomponent
