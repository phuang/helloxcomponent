#include "hello/AVPlayer.h"

#include "hello/Log.h"
#include "hello/NativeWindow.h"

namespace hello {
namespace {
constexpr uint64_t kUsage = NATIVEBUFFER_USAGE_HW_RENDER |
                            NATIVEBUFFER_USAGE_HW_TEXTURE |
                            NATIVEBUFFER_USAGE_CPU_WRITE;
}

AVPlayer::AVPlayer(const std::string& url) : url_(url) {}

AVPlayer::~AVPlayer() {
  if (player_) {
    OH_AVPlayer_ReleaseSync(player_);
  }
}

bool AVPlayer::Initialize(NativeWindow* window) {
  native_window_ = window;

  player_ = OH_AVPlayer_Create();
  FATAL_IF(player_ == nullptr, "OH_AVPlayer_Create()");

  OH_AVErrCode ret;
  ret = OH_AVPlayer_SetOnInfoCallback(
      player_,
      [](OH_AVPlayer* player, AVPlayerOnInfoType type, OH_AVFormat* info,
         void* user_data) {
        reinterpret_cast<AVPlayer*>(user_data)->OnInfo(type, info);
      },
      this);
  FATAL_IF(ret != AV_ERR_OK, "OH_AVPlayer_SetOnInfoCallback() ret:%{public}d",
           ret);

  ret = OH_AVPlayer_SetOnErrorCallback(
      player_,
      [](OH_AVPlayer* player, int32_t code, const char* message,
         void* user_data) {
        reinterpret_cast<AVPlayer*>(user_data)->OnError(code, message);
      },
      this);

  ret = OH_AVPlayer_SetURLSource(player_, url_.c_str());
  FATAL_IF(ret != AV_ERR_OK, "OH_AVPlayer_SetURLSource() ret:%{public}d", ret);

  return true;
}

void AVPlayer::OnInfo(AVPlayerOnInfoType type, OH_AVFormat* info) {
  switch (type) {
    case AV_INFO_TYPE_STATE_CHANGE: {
      int32_t state;
      OH_AVFormat_GetIntValue(info, OH_PLAYER_STATE, &state);
      OnStateChange(static_cast<AVPlayerState>(state));
      break;
    }
    case AV_INFO_TYPE_RESOLUTION_CHANGE: {
      int32_t ret;
      ret = OH_AVPlayer_GetVideoWidth(player_, &width_);
      FATAL_IF(ret != AV_ERR_OK, "OH_AVPlayer_GetVideoWidth() ret:%{public}d",
               ret);

      ret = OH_AVPlayer_GetVideoHeight(player_, &height_);
      FATAL_IF(ret != AV_ERR_OK, "OH_AVPlayer_GetVideoHeight() ret:%{public}d",
               ret);
      break;
    }
    default: {
      break;
    }
  }
}

void AVPlayer::OnError(int32_t code, const char* message) {
  LOGE("OnError() %{public}d %{public}s", code, message);
}

void AVPlayer::OnStateChange(AVPlayerState state) {
  OH_AVErrCode ret;
  state_ = state;
  switch (state) {
    case AV_IDLE:
      break;
    case AV_INITIALIZED: {
      ret = OH_AVPlayer_SetVideoSurface(player_, native_window_->window());
      FATAL_IF(ret != AV_ERR_OK, "OH_AVPlayer_SetVideoSurface() ret:%d", ret);
      ret = OH_AVPlayer_Prepare(player_);
      FATAL_IF(ret != AV_ERR_OK, "OH_AVPlayer_Prepare() ret:%{public}d", ret);
      break;
    }
    case AV_PREPARED: {
      ret = OH_AVPlayer_SetLooping(player_, true);
      FATAL_IF(ret != AV_ERR_OK, "OH_AVPlayer_SetLooping() ret:%{public}d",
               ret);

      if (play_) {
        ret = OH_AVPlayer_Play(player_);
        FATAL_IF(ret != AV_ERR_OK, "OH_AVPlayer_Play() ret:%{public}d", ret);
      }
      break;
    }
    case AV_PLAYING:
    case AV_PAUSED:
    case AV_STOPPED:
    case AV_COMPLETED:
    default:
      break;
  }
}

void AVPlayer::SetNativeWindow(NativeWindow* window) {
  Initialize(window);
}

void AVPlayer::StartDrawFrame() {
  if (!play_) {
    play_ = true;
    switch (state_) {
      case AV_PREPARED:
      case AV_PAUSED:
      case AV_STOPPED: {
        int32_t ret = OH_AVPlayer_Play(player_);
        FATAL_IF(ret != AV_ERR_OK, "OH_AVPlayer_Play() ret:%{public}d", ret);
        break;
      }
      default:
        break;
    }
  }
}

void AVPlayer::StopDrawFrame() {
  if (play_) {
    play_ = false;
    if (state_ == AV_PLAYING) {
      int32_t ret = OH_AVPlayer_Pause(player_);
      FATAL_IF(ret != AV_ERR_OK, "OH_AVPlayer_Pause() ret:%{public}d", ret);
    }
  }
}

}  // namespace hello
