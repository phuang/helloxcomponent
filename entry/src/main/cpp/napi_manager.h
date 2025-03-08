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
