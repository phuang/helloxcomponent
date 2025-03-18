#ifndef HELLO_DISPLAYMANAGER_H_
#define HELLO_DISPLAYMANAGER_H_

#include <window_manager/oh_display_manager.h>

namespace hello {
class DisplayManager {
 public:
  DisplayManager();
  ~DisplayManager();

  int32_t width() const { return info_->width; }
  int32_t height() const { return info_->height; }
  int32_t physical_width() const { return info_->physicalWidth; }
  int32_t physical_height() const { return info_->physicalHeight; }
  int32_t available_width() const { return info_->availableWidth; }
  int32_t available_height() const { return info_->availableHeight; }
  float density_dpi() const { return info_->densityDPI; }
  float density_pixels() const { return info_->densityPixels; }
  float scaled_density() const { return info_->scaledDensity; }
  float x_dpi() const { return info_->xDPI; }
  float y_dpi() const { return info_->yDPI; }

 private:
  NativeDisplayManager_DisplayInfo* info_ = nullptr;
};
}  // namespace hello

#endif  // HELLO_DISPLAYMANAGER_H_
