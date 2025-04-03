#ifndef HELLOXCOMPONENT_NAPI_MANAGER_H_
#define HELLOXCOMPONENT_NAPI_MANAGER_H_

#include "hello/NodeContent.h"

#define NODE_ADDON_API_DISABLE_DEPRECATED
#include "napi.h"

namespace hello {

class DisplayManager;
class GLCore;

class NapiManager {
 public:
  static void Init(const Napi::Env& env);
  static NapiManager* GetInstance();
  static Napi::Value NapiCreateNativeNode(const Napi::CallbackInfo& info);
  static Napi::Value NapiSetCurrentMode(
      const Napi::CallbackInfo& info);
  static Napi::Value NapiOnPageShow(const Napi::CallbackInfo& info);
  static Napi::Value NapiOnPageHide(const Napi::CallbackInfo& info);

  const Napi::Env& env() const { return env_; }
  GLCore* gl_core() const { return gl_core_.get(); }
  DisplayManager* display_manager() const { return display_manager_.get(); }

 private:
  NapiManager(const Napi::Env& env);
  ~NapiManager();

  NapiManager(const NapiManager&) = delete;
  NapiManager& operator=(const NapiManager&) = delete;
  NapiManager(NapiManager&&) = delete;
  NapiManager& operator=(NapiManager&&) = delete;

  void CreateNativeNode(ArkUI_NodeContentHandle content_handle, int mode);
  void SetCurrentMode(int mode);
  void OnPageShow();
  void OnPageHide();
  void Update();

  Napi::Env env_{nullptr};

  std::unique_ptr<NodeContent> delegated_node_content_;
  std::unique_ptr<NodeContent> non_delegated_node_content_;
  std::unique_ptr<NodeContent> surface_control_node_content_;

  int current_mode_ = 0;
  bool is_visible_ = false;

  // Hold a weak reference of the controller
  Napi::Reference<Napi::Object> controller_;

  std::unique_ptr<GLCore> gl_core_;
  std::unique_ptr<DisplayManager> display_manager_;
};

}  // namespace hello

#endif  // HELLOXCOMPONENT_NAPI_MANAGER_H_
