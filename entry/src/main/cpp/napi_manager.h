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

#ifndef HELLOXCOMPONENT_NAPI_MANAGER_H_
#define HELLOXCOMPONENT_NAPI_MANAGER_H_

#include "hello/XComponentNode.h"
#define NODE_ADDON_API_DISABLE_DEPRECATED
#include <napi/native_api.h>

#include <string>
#include <unordered_map>

#include "napi.h"

struct OH_NativeXComponent;

namespace hello {
class XComponentNode;
}

namespace helloxcomponent {

class NapiManager {
 public:
  static NapiManager* GetInstance();

  // Napi export
  bool Export(Napi::Env env, Napi::Object exports);

  static Napi::Value GetContext(const Napi::CallbackInfo& info);
  static Napi::Value NapiCreateNativeNode(const Napi::CallbackInfo& info);

  void set_controller(Napi::Reference<Napi::Object> controller) {
    controller_ = std::move(controller);
  }

  OH_NativeXComponent* root_view() const {
    return root_view_;
  }
  
 private:
  NapiManager();
  ~NapiManager();

  /**APP Lifecycle**/
  static Napi::Value NapiOnCreate(const Napi::CallbackInfo& info);
  static Napi::Value NapiOnShow(const Napi::CallbackInfo& info);
  static Napi::Value NapiOnHide(const Napi::CallbackInfo& info);
  static Napi::Value NapiOnDestroy(const Napi::CallbackInfo& info);

  void OnCreateNative(Napi::Env env);
  void OnShowNative();
  void OnHideNative();
  void OnDestroyNative();

  /**JS Page Lifecycle**/
  static Napi::Value NapiOnPageShow(const Napi::CallbackInfo& info);
  static Napi::Value NapiOnPageHide(const Napi::CallbackInfo& info);

  void OnPageShowNative();
  void OnPageHideNative();
  
  void CreateNativeNode(ArkUI_NodeContentHandle content_handle);

  void SetRootViewXComp(OH_NativeXComponent* root_view) {
    root_view_ = root_view;
  }

  Napi::Env env_{nullptr};
  
  std::unique_ptr<hello::XComponentNode> root_node_;
  
  std::string id_;
  
  OH_NativeXComponent* root_view_ = nullptr;

  // Hold a weak reference of the controller
  Napi::Reference<Napi::Object> controller_;
};

}  // namespace helloxcomponent

#endif  // HELLOXCOMPONENT_NAPI_MANAGER_H_
