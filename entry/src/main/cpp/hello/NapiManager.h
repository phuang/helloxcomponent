#ifndef HELLOXCOMPONENT_NAPI_MANAGER_H_
#define HELLOXCOMPONENT_NAPI_MANAGER_H_

#include "hello/NodeContent.h"

#define NODE_ADDON_API_DISABLE_DEPRECATED
#include "napi.h"

namespace hello {

class GLCore;
class AVPlayer;

class NapiManager {
 public:
  static void Init(const Napi::Env& env);
  static NapiManager* GetInstance();
  static Napi::Value NapiCreateNativeNode(const Napi::CallbackInfo& info);
  static Napi::Value NapiSetDelegatedCompositing(
      const Napi::CallbackInfo& info);

  const Napi::Env& env() const { return env_; }
  GLCore* gl_core() const { return gl_core_.get(); }
  AVPlayer* av_player() const { return av_player_.get(); }

 private:
  NapiManager(const Napi::Env& env);
  ~NapiManager();

  void CreateNativeNode(ArkUI_NodeContentHandle content_handle, bool delegated);
  void SetDelegatedCompositing(bool enable);

  Napi::Env env_{nullptr};

  std::unique_ptr<NodeContent> delegated_node_content_;
  std::unique_ptr<NodeContent> non_delegated_node_content_;

  // Hold a weak reference of the controller
  Napi::Reference<Napi::Object> controller_;

  std::unique_ptr<GLCore> gl_core_;

  std::unique_ptr<AVPlayer> av_player_;
};

}  // namespace hello

#endif  // HELLOXCOMPONENT_NAPI_MANAGER_H_
