#ifndef HELLOXCOMPONENT_SURFACECONTROL_H
#define HELLOXCOMPONENT_SURFACECONTROL_H

#include "hello/NodeContent.h"

#include <bitset>
#include <memory>
#include <vector>

#include "surface_control/ndk/surface_control.h"

namespace hello {

class BufferQueue;
class NativeBuffer;
class Renderer;
class Thread;

class SurfaceControl {
 public:
  static std::unique_ptr<SurfaceControl> Create(const char* name,
                                                NativeWindow* parent,
                                                int32_t width,
                                                int32_t height,
                                                bool is_software,
                                                Renderer* renderer);

  static std::unique_ptr<SurfaceControl> Create(const char* name,
                                                int32_t width,
                                                int32_t height,
                                                bool is_software,
                                                Renderer* renderer);

  ~SurfaceControl();

  bool Update(OH_SurfaceTransaction* transaction);

 private:
  SurfaceControl(const SurfaceControl&) = delete;
  SurfaceControl(SurfaceControl&&) = delete;
  SurfaceControl& operator=(const SurfaceControl&) = delete;
  SurfaceControl& operator=(SurfaceControl&&) = delete;

  SurfaceControl(OH_SurfaceControl* surface,
                 int32_t width,
                 int32_t height,
                 bool is_software,
                 Renderer* renderer);

  void SoftwareDrawFrame();
  void HardwareDrawFrame();

  void OnBufferRelease(std::shared_ptr<NativeBuffer> buffer);

  OH_SurfaceControl* surface_ = nullptr;
  const bool is_software_;
  Renderer* const renderer_;
  enum {
    kDirtyBitPosition = 0,
    kDirtyBitSize = 0,
    kDirtyBitCount,
  };
  std::bitset<kDirtyBitCount> dirty_bits_;
  int32_t x_ = 0;
  int32_t y_ = 0;
  int32_t width_ = 0;
  int32_t height_ = 0;
  std::shared_ptr<BufferQueue> buffer_queue_;
  std::unique_ptr<Thread> renderer_thread_;
};

}  // namespace hello

#endif  // HELLOXCOMPONENT_SURFACECONTROL_H
