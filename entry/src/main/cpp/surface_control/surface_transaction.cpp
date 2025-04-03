#include "surface_control/surface_transaction.h"

#include "commonlibrary/c_utils/base/include/unique_fd.h"
#include "foundation/graphic/graphic_surface/interfaces/inner_api/surface/surface_buffer.h"
#include "surface_control/ndk/surface_control.h"

namespace OHOS::surface_control {

SurfaceTransaction::SurfaceTransaction() = default;
SurfaceTransaction::~SurfaceTransaction() = default;

void SurfaceTransaction::Commit() {
  for (auto& command : transaction_commands_) {
    command();
  }
  transaction_commands_.clear();

  for (auto surface : surface_controls_) {
    surface->SyncBufferToNodeIfNecessary();
  }
  surface_controls_.clear();
}

void SurfaceTransaction::SetOnComplete(const OnCompleteCallback& callback) {
  on_complete_callback_ = callback;
}

void SurfaceTransaction::SetOnCommit(const OnCommitCallback& callback) {
  on_commit_callback_ = callback;
}

void SurfaceTransaction::Reparent(SurfaceControl* surface_control,
                                  SurfaceControl* new_parent) {
  transaction_commands_.push_back(
      [surface = sptr<SurfaceControl>(surface_control),
       parent = sptr<SurfaceControl>(new_parent)] {
        surface->SetParent(parent.GetRefPtr());
      });
}

void SurfaceTransaction::SetVisibility(SurfaceControl* surface_control,
                                       bool visibility) {
  transaction_commands_.push_back(
      [surface = sptr<SurfaceControl>(surface_control), visibility] {
        surface->SetVisibility(visibility);
      });
}

void SurfaceTransaction::SetZOrder(SurfaceControl* surface_control,
                                   int32_t z_order) {
  transaction_commands_.push_back(
      [surface = sptr<SurfaceControl>(surface_control), z_order] {
        surface->SetZOrder(z_order);
      });
}

void SurfaceTransaction::SetBuffer(SurfaceControl* surface_control,
                                   SurfaceBuffer* buffer,
                                   int acquire_fence_fd,
                                   const BufferReleaseCallback& callback) {
  transaction_commands_.push_back([surface = surface_control,
                                   surface_buffer = sptr<SurfaceBuffer>(buffer),
                                   acquire_fence_fd, callback] {
    surface->SetBuffer(std::move(surface_buffer), UniqueFd(acquire_fence_fd),
                       callback);
  });
  surface_controls_.insert(surface_control);
}

void SurfaceTransaction::SetCrop(SurfaceControl* surface_control,
                                 const OH_Rect* crop) {
  static_assert(sizeof(Rect) == sizeof(OH_Rect));
  transaction_commands_.push_back(
      [surface = sptr<SurfaceControl>(surface_control),
       crop = *reinterpret_cast<const Rect*>(crop)] {
        surface->SetCrop(&crop);
      });
}

void SurfaceTransaction::SetPosition(SurfaceControl* surface_control,
                                     int32_t x,
                                     int32_t y) {
  transaction_commands_.push_back(
      [surface = sptr<SurfaceControl>(surface_control), x, y] {
        surface->SetPosition(x, y);
      });
}

void SurfaceTransaction::SetBufferTransform(SurfaceControl* surface_control,
                                            int32_t transform) {
  transaction_commands_.push_back([surface = surface_control, transform] {
    surface->SetBufferTransform(transform);
  });
  // Buffer thrasform change may need to set buffer again.
  surface_controls_.insert(surface_control);
}

void SurfaceTransaction::SetScale(SurfaceControl* surface_control,
                                  float x_scale,
                                  float y_scale) {
  transaction_commands_.push_back(
      [surface = sptr<SurfaceControl>(surface_control), x_scale, y_scale] {
        surface->SetScale(x_scale, y_scale);
      });
}

void SurfaceTransaction::SetBufferTransparency(SurfaceControl* surface_control,
                                               int32_t transparency) {
  transaction_commands_.push_back(
      [surface = sptr<SurfaceControl>(surface_control), transparency] {
        surface->SetBufferTransparency(transparency);
      });
}

void SurfaceTransaction::SetDamageRegion(SurfaceControl* surface_control,
                                         const OH_Rect* rects,
                                         uint32_t count) {
  std::shared_ptr<std::vector<Rect>> damage_rects;
  if (count != 0) {
    damage_rects = std::make_shared<std::vector<Rect>>();
    damage_rects->resize(count);
  }
  static_assert(sizeof(Rect) == sizeof(OH_Rect));
  memcpy(damage_rects->data(), rects, sizeof(Rect) * count);
  transaction_commands_.push_back(
      [surface = sptr<SurfaceControl>(surface_control), damage_rects, count] {
        surface->SetDamageRegion(damage_rects ? std::move(*damage_rects)
                                              : std::vector<Rect>());
      });
}

void SurfaceTransaction::SetDesiredPresentTime(int64_t desired_present_time) {
  desired_present_time_ = desired_present_time;
}

void SurfaceTransaction::SetBufferAlpha(SurfaceControl* surface_control,
                                        float alpha) {
  transaction_commands_.push_back(
      [surface = sptr<SurfaceControl>(surface_control), alpha] {
        surface->SetBufferAlpha(alpha);
      });
}

void SurfaceTransaction::SetFrameRateWithChangeStrategy(
    SurfaceControl* surface_control,
    float frame_rate,
    int8_t compatibility,
    int32_t strategy) {
  transaction_commands_.push_back(
      [surface = sptr<SurfaceControl>(surface_control), frame_rate,
       compatibility, strategy] {
        surface->SetFrameRateWithChangeStrategy(frame_rate, compatibility,
                                                strategy);
      });
}

void SurfaceTransaction::ClearFrameRate(SurfaceControl* surface_control) {
  transaction_commands_.push_back(
      [surface = sptr<SurfaceControl>(surface_control)] {
        surface->ClearFrameRate();
      });
}

void SurfaceTransaction::SetEnableBackPressure(SurfaceControl* surface_control,
                                               bool enable_back_pressure) {
  transaction_commands_.push_back(
      [surface = sptr<SurfaceControl>(surface_control), enable_back_pressure] {
        surface->SetEnableBackPressure(enable_back_pressure);
      });
}

}  // namespace OHOS::surface_control
