

#define NODE_ADDON_API_DISABLE_DEPRECATED
#include "napi.h"
#include "napi_manager.h"

namespace helloxcomponent {

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  std::srand(std::time({}));
  exports.Set("createNativeNode", Napi::Function::New<NapiManager::NapiCreateNativeNode>(env));
  return exports;
}

} // namespace helloxcomponent

using helloxcomponent::Init;

NODE_API_MODULE(helloxcomponent, Init)
