

#define NODE_ADDON_API_DISABLE_DEPRECATED
#include "napi.h"
#include "hello/NapiManager.h"

namespace helloxcomponent {

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  hello::NapiManager::Init(env);
  exports.Set("createNativeNode",
              Napi::Function::New<hello::NapiManager::NapiCreateNativeNode>(env));
  exports.Set(
      "setDelegatedCompositing",
      Napi::Function::New<hello::NapiManager::NapiSetDelegatedCompositing>(env));
  return exports;
}

}  // namespace helloxcomponent

using helloxcomponent::Init;

NODE_API_MODULE(helloxcomponent, Init)
