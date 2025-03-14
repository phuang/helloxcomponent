#ifndef HELLO_AVPLAYER_H_
#define HELLO_AVPLAYER_H_

#include <multimedia/player_framework/avplayer.h>

#include <memory>

namespace hello {

class NativeWindow;

class AVPlayer {
 public:
  AVPlayer();
  ~AVPlayer();

private:
  bool Initialize();
  void OnInfo(AVPlayerOnInfoType type, OH_AVFormat* info);
  void OnError(int32_t code, const char* message);
  void OnStateChange(AVPlayerState state);

  OH_AVPlayer* player_ = nullptr;
  int32_t width_ = 0;
  int32_t height_ = 0;

  std::unique_ptr<NativeWindow> native_window_;
  std::unique_ptr<NativeWindow> native_window_player_;
};

}  // namespace hello

#endif
