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

#include <string>

#define NODE_ADDON_API_DISABLE_DEPRECATED
#include "napi.h"

namespace hello {
class XComponentNode;
}

namespace helloxcomponent {

class NapiManager {
  public:
  static NapiManager *GetInstance();
  static Napi::Value NapiCreateNativeNode(const Napi::CallbackInfo &info);

  private:
  NapiManager();
  ~NapiManager();

  void CreateNativeNode(ArkUI_NodeContentHandle content_handle);

  Napi::Env env_{nullptr};

  std::unique_ptr<hello::XComponentNode> root_node_;

  std::string id_;

  // Hold a weak reference of the controller
  Napi::Reference<Napi::Object> controller_;
};

} // namespace helloxcomponent

#endif // HELLOXCOMPONENT_NAPI_MANAGER_H_
