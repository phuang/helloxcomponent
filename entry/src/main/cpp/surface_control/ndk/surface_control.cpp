#include "surface_control/ndk/surface_control.h"

#include <memory>

#include "foundation/graphic/graphic_surface/interfaces/inner_api/surface/surface_buffer.h"
#include "surface_control/surface_control.h"
#include "surface_control/surface_transaction.h"

#ifdef __cplusplus
extern "C" {
#endif

using namespace OHOS::surface_control;

OH_SurfaceControl* OH_SurfaceControl_FromNativeWindow(OHNativeWindow* parent,
                                                      const char* debug_name) {
  return nullptr;
}

OH_SurfaceControl* OH_SurfaceControl_create(const char* debug_name) {
  return nullptr;
}

void OH_SurfaceControl_acquire(OH_SurfaceControl* surface_control) {}

void OH_SurfaceControl_release(OH_SurfaceControl* surface_control) {}

OH_SurfaceTransaction* OH_SurfaceTransaction_Create() {
  auto* txn = new SurfaceTransaction();
  return reinterpret_cast<OH_SurfaceTransaction*>(txn);
}

int64_t OH_SurfaceTransactionStats_GetLatchTime(
    OH_SurfaceTransactionStats* surface_transaction_stats) {
  return 0;
}

int OH_SurfaceTransactionStats_GetPresentFenceFd(
    OH_SurfaceTransactionStats* surface_transaction_stats) {
  return -1;
}

void OH_SurfaceTransactionStats_GetSurfaceControls(
    OH_SurfaceTransactionStats* surface_transaction_stats,
    OH_SurfaceControl*** out_surface_controls,
    size_t* out_surface_controls_size) {}

void OH_SurfaceTransactionStats_ReleaseSurfaceControls(
    OH_SurfaceControl** surface_controls) {}

void OH_SurfaceTransaction_Delete(OH_SurfaceTransaction* transaction) {
  auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
  delete reinterpret_cast<SurfaceTransaction*>(transaction);
}

void OH_SurfaceTransaction_Commit(OH_SurfaceTransaction* transaction) {
  auto txn = reinterpret_cast<SurfaceTransaction*>(transaction);
  txn->Commit();
}

void OH_SurfaceTransaction_SetOnComplete(
    OH_SurfaceTransaction* transaction,
    void* context,
    OH_SurfaceTransaction_OnComplete func) {
  auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
  if (func) {
    txn->SetOnComplete([context, func](SurfaceTransactionStats* state) {
      func(context, reinterpret_cast<OH_SurfaceTransactionStats*>(state));
    });
  } else {
    txn->SetOnComplete({});
  }
}

void OH_SurfaceTransaction_SetOnCommit(OH_SurfaceTransaction* transaction,
                                       void* context,
                                       OH_SurfaceTransaction_OnCommit func) {
  auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
  if (func) {
    txn->SetOnCommit([context, func](SurfaceTransactionStats* state) {
      func(context, reinterpret_cast<OH_SurfaceTransactionStats*>(state));
    });
  } else {
    txn->SetOnCommit({});
  }
}

void OH_SurfaceTransaction_Reparent(OH_SurfaceTransaction* transaction,
                                    OH_SurfaceControl* surface_control,
                                    OH_SurfaceControl* new_parent) {
  auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
  auto* surface = reinterpret_cast<SurfaceControl*>(surface_control);
  auto* parent = reinterpret_cast<SurfaceControl*>(new_parent);
  txn->Reparent(surface, parent);
}

void OH_SurfaceTransaction_SetVisibility(
    OH_SurfaceTransaction* transaction,
    OH_SurfaceControl* surface_control,
    enum OH_SurfaceTransactionVisibility visibility) {
  auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
  auto* surface = reinterpret_cast<SurfaceControl*>(surface_control);
  txn->SetVisibility(surface,
                     visibility == OH_SURFACE_TRANSACTION_VISIBILITY_SHOW);
}

void OH_SurfaceTransaction_SetZOrder(OH_SurfaceTransaction* transaction,
                                     OH_SurfaceControl* surface_control,
                                     int32_t z_order) {
  auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
  auto* surface = reinterpret_cast<SurfaceControl*>(surface_control);
  txn->SetZOrder(surface, z_order);
}

void OH_SurfaceTransaction_setBufferWithRelease(
    OH_SurfaceTransaction* transaction,
    OH_SurfaceControl* surface_control,
    OH_NativeBuffer* buffer,
    int acquire_fence_fd,
    void* context,
    OH_SurfaceTransaction_OnBufferRelease func) {
  auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
  auto* surface = reinterpret_cast<SurfaceControl*>(surface_control);
  auto* surface_buffer =
      OHOS::SurfaceBuffer::NativeBufferToSurfaceBuffer(buffer);

  txn->SetBuffer(
      surface, surface_buffer, acquire_fence_fd,
      [context, func](int releaseFenceFd) { func(context, releaseFenceFd); });
}

void OH_SurfaceTransaction_setCrop(OH_SurfaceTransaction* transaction,
                                   OH_SurfaceControl* surface_control,
                                   const OH_Rect* crop) {
  auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
  auto* surface = reinterpret_cast<SurfaceControl*>(surface_control);
  txn->SetCrop(surface, crop);
}

void OH_SurfaceTransaction_SetPosition(OH_SurfaceTransaction* transaction,
                                       OH_SurfaceControl* surface_control,
                                       int32_t x,
                                       int32_t y) {
  auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
  auto* surface = reinterpret_cast<SurfaceControl*>(surface_control);
  txn->SetPosition(surface, x, y);
}

void OH_SurfaceTransaction_SetBufferTransform(
    OH_SurfaceTransaction* transaction,
    OH_SurfaceControl* surface_control,
    int32_t transform) {
  auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
  auto* surface = reinterpret_cast<SurfaceControl*>(surface_control);
  txn->SetBufferTransform(surface, transform);
}

void OH_SurfaceTransaction_SetScale(OH_SurfaceTransaction* transaction,
                                    OH_SurfaceControl* surface_control,
                                    float xScale,
                                    float yScale) {
  auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
  auto* surface = reinterpret_cast<SurfaceControl*>(surface_control);
  txn->SetScale(surface, xScale, yScale);
}

void OH_SurfaceTransaction_SetBufferTransparency(
    OH_SurfaceTransaction* transaction,
    OH_SurfaceControl* surface_control,
    enum OH_SurfaceTransactionTransparency transparency) {
  auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
  auto* surface = reinterpret_cast<SurfaceControl*>(surface_control);
  txn->SetBufferTransparency(surface, transparency);
}

void OH_SurfaceTransaction_SetDamageRegion(OH_SurfaceTransaction* transaction,
                                           OH_SurfaceControl* surface_control,
                                           const OH_Rect* rects,
                                           uint32_t count) {
  auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
  auto* surface = reinterpret_cast<SurfaceControl*>(surface_control);
  txn->SetDamageRegion(surface, rects, count);
}

void OH_SurfaceTransaction_SetDesiredPresentTime(
    OH_SurfaceTransaction* transaction,
    int64_t desired_presentTime) {
  auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
  txn->SetDesiredPresentTime(desired_presentTime);
}

void OH_SurfaceTransaction_SetBufferAlpha(OH_SurfaceTransaction* transaction,
                                          OH_SurfaceControl* surface_control,
                                          float alpha) {
  auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
  auto* surface = reinterpret_cast<SurfaceControl*>(surface_control);
  txn->SetBufferAlpha(surface, alpha);
}

void OH_SurfaceTransaction_SetFrameRateWithChangeStrategy(
    OH_SurfaceTransaction* transaction,
    OH_SurfaceControl* surface_control,
    float frameRate,
    int8_t compatibility,
    int8_t changeFrameRateStrategy) {
  auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
  auto* surface = reinterpret_cast<SurfaceControl*>(surface_control);
  txn->SetFrameRateWithChangeStrategy(surface, frameRate, compatibility,
                                      changeFrameRateStrategy);
}

void OH_SurfaceTransaction_ClearFrameRate(OH_SurfaceTransaction* transaction,
                                          OH_SurfaceControl* surface_control) {
  auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
  auto* surface = reinterpret_cast<SurfaceControl*>(surface_control);
  txn->ClearFrameRate(surface);
}

void OH_SurfaceTransaction_SetEnableBackPressure(
    OH_SurfaceTransaction* transaction,
    OH_SurfaceControl* surface_control,
    bool enableBackPressure) {
  auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
  auto* surface = reinterpret_cast<SurfaceControl*>(surface_control);
  txn->SetEnableBackPressure(surface, enableBackPressure);
}

#ifdef __cplusplus
}
#endif
