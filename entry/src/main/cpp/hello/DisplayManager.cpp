#include "hello/DisplayManager.h"

#include "hello/Log.h"

namespace hello {

DisplayManager::DisplayManager() {
  NativeDisplayManager_ErrorCode ret;
  ret = OH_NativeDisplayManager_CreatePrimaryDisplay(&info_);
  FATAL_IF(ret != DISPLAY_MANAGER_OK,
           "OH_NativeDisplayManager_CreatePrimaryDisplay(): %{public}d", ret);

  LOGE("EEEE DM: size=%{public}dx%{public}d", width(), height());
  LOGE("EEEE DM: physical_size=%{public}dx%{public}d", physical_width(), physical_height());
  LOGE("EEEE DM: available_size=%{public}dx%{public}d", available_width(), available_height());
  LOGE("EEEE DM: density_dpi=%{public}f", density_dpi());
  LOGE("EEEE DM: density_pixels=%{public}f", density_pixels());
  LOGE("EEEE DM: scaled_density=%{public}f", scaled_density());
  LOGE("EEEE DM: x_dpi=%{public}f", x_dpi());
  LOGE("EEEE DM: y_dpi=%{public}f", y_dpi());
}

DisplayManager::~DisplayManager() {
  OH_NativeDisplayManager_DestroyDisplay(info_);
}

}  // namespace hello
