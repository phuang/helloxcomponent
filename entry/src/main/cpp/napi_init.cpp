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
namespace {

Napi::Value SetController(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (info.Length() != 1) {
    return Napi::Boolean::New(env, false);
  }

  if (!info[0].IsObject()) {
    return Napi::Boolean::New(env, false);
  }

  Napi::Object obj = info[0].As<Napi::Object>();
  NapiManager::GetInstance()->set_controller(Napi::Reference<Napi::Object>::New(obj));

  return Napi::Boolean::New(env, true);
}

} // namespace

Napi::Object Init(Napi::Env env, Napi::Object exports) {

  exports.Set("setController", Napi::Function::New<SetController>(env));
  exports.Set("createNativeNode", Napi::Function::New<NapiManager::NapiCreateNativeNode>(env));
  exports.Set("getContext", Napi::Function::New<NapiManager::GetContext>(env));

//  NapiManager::GetInstance()->Export(env, exports);
  return exports;
}

} // namespace helloxcomponent

using helloxcomponent::Init;

NODE_API_MODULE(helloxcomponent, Init)
