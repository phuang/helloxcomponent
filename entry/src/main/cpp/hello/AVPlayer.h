#ifndef HELLO_AVPLAYER_H_
#define HELLO_AVPLAYER_H_

#include <multimedia/player_framework/avplayer.h>

#include <memory>

#include "hello/XComponentNode.h"

namespace hello {

class NativeWindow;

class AVPlayer : public XComponentNode::Delegate {
 public:
  explicit AVPlayer(const std::string& url);
  ~AVPlayer() override;

  void SetNativeWindow(NativeWindow* window) override;
  void StartDrawFrame() override;
  void StopDrawFrame() override;

 private:
  bool Initialize(NativeWindow* window);
  void OnInfo(AVPlayerOnInfoType type, OH_AVFormat* info);
  void OnError(int32_t code, const char* message);
  void OnStateChange(AVPlayerState state);

  const std::string url_;
  OH_AVPlayer* player_ = nullptr;
  int32_t width_ = 0;
  int32_t height_ = 0;
  NativeWindow* native_window_ = nullptr;
  bool play_ = false;
  AVPlayerState state_ = AV_IDLE;
};

}  // namespace hello

#endif
