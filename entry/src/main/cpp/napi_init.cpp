#include <napi/native_api.h>
#include <sys/mman.h>
#include <uv.h>

#include <functional>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#define NODE_ADDON_API_DISABLE_DEPRECATED
#include "napi.h"
#include "napi_manager.h"


namespace helloxcomponent {

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set("createNativeNode", Napi::Function::New<NapiManager::NapiCreateNativeNode>(env));
  return exports;
}

} // namespace helloxcomponent

using helloxcomponent::Init;

NODE_API_MODULE(helloxcomponent, Init)
