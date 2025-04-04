#ifndef SURFACE_CONTROL_SURFACE_TRANSACTION_H_
#define SURFACE_CONTROL_SURFACE_TRANSACTION_H_

#include <functional>
#include <set>
#include <vector>

#include "surface_control/surface_control.h"

struct OH_Rect;

namespace OHOS::surface_control {

class SurfaceTransactionStats {
 public:
  SurfaceTransactionStats() = default;
  ~SurfaceTransactionStats() = default;
  int64_t GetLatchTime() const;
  int GetPresentFenceFd() const;
  void GetSurfaceControls(SurfaceControl*** out_surface_controls,
                          size_t* out_surface_controls_size) const;
  void ReleaseSurfaceControls(SurfaceControl** surface_controls) const;

 private:
  SurfaceTransactionStats(const SurfaceTransactionStats&) = delete;
  SurfaceTransactionStats& operator=(const SurfaceTransactionStats&) = delete;
  SurfaceTransactionStats(SurfaceTransactionStats&&) = delete;
  SurfaceTransactionStats& operator=(SurfaceTransactionStats&&) = delete;
};

class SurfaceTransaction {
 public:
  using OnCompleteCallback =
      std::function<void(SurfaceTransactionStats* state)>;
  using OnCommitCallback = std::function<void(SurfaceTransactionStats* state)>;
  using BufferReleaseCallback = SurfaceControl::BufferReleaseCallback;

  SurfaceTransaction();
  ~SurfaceTransaction();

  void Commit();

  void SetOnComplete(const OnCompleteCallback& callback);
  void SetOnCommit(const OnCommitCallback& callback);
  void SetDesiredPresentTime(int64_t desired_present_time);
  void Reparent(SurfaceControl* surface_control, SurfaceControl* new_parent);
  void SetVisibility(SurfaceControl* surface_control, bool visibility);
  void SetZOrder(SurfaceControl* surface_control, int32_t z_order);
  void SetBuffer(SurfaceControl* surface_control,
                 SurfaceBuffer* buffer,
                 int acquire_fence_fd,
                 const BufferReleaseCallback& callback);
  void SetCrop(SurfaceControl* surface_control, const OH_Rect* crop);
  void SetPosition(SurfaceControl* surface_control, int32_t x, int32_t y);
  void SetBufferTransform(SurfaceControl* surface_control, int32_t transform);
  void SetScale(SurfaceControl* surface_control, float x_scale, float y_scale);
  void SetRotation(SurfaceControl* surface_control,
                   float degree_x,
                   float degree_y,
                   float degree_z);
  void SetBufferTransparency(SurfaceControl* surface_control,
                             int32_t transparency);
  void SetDamageRegion(SurfaceControl* surface_control,
                       const OH_Rect* rects,
                       uint32_t count);
  void SetBufferAlpha(SurfaceControl* surface_control, float alpha);
  void SetFrameRateWithChangeStrategy(SurfaceControl* surface_control,
                                      float frame_rate,
                                      int8_t compatibility,
                                      int32_t strategy);
  void ClearFrameRate(SurfaceControl* surface_control);
  void SetEnableBackPressure(SurfaceControl* surface_control,
                             bool enable_back_pressure);

 private:
  SurfaceTransaction(const SurfaceTransaction&) = delete;
  SurfaceTransaction& operator=(const SurfaceTransaction&) = delete;
  SurfaceTransaction(SurfaceTransaction&&) = delete;
  SurfaceTransaction& operator=(SurfaceTransaction&&) = delete;
  OnCompleteCallback on_complete_callback_;
  OnCommitCallback on_commit_callback_;
  int64_t desired_present_time_ = 0;
  std::vector<std::function<void()>> transaction_commands_;
  std::set<sptr<SurfaceControl>> surface_controls_;
};

}  // namespace OHOS::surface_control

#endif  // SURFACE_CONTROL_SURFACE_TRANSACTION_H_
