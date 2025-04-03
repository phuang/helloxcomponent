#ifndef SURFACE_CONTROL_SURFACE_CONTROL_H_
#define SURFACE_CONTROL_SURFACE_CONTROL_H_

#include <refbase.h>

#include <memory>
#include <vector>

#include "commonlibrary/c_utils/base/include/unique_fd.h"
#include "foundation/graphic/graphic_surface/interfaces/inner_api/surface/surface_type.h"

struct NativeWindow;

namespace OHOS {
class SurfaceBuffer;
}  // namespace OHOS

namespace OHOS::Rosen {
class RSNode;
class RSProxyNode;
class RSSurfaceNode;
};  // namespace OHOS::Rosen

namespace OHOS::surface_control {

class SurfaceControl : public RefBase {
 public:
  using BufferReleaseCallback = std::function<void(int release_fence_fd)>;

  SurfaceControl(std::shared_ptr<OHOS::Rosen::RSSurfaceNode> surface_node,
                 std::shared_ptr<OHOS::Rosen::RSNode> parent = {});
  ~SurfaceControl() override;

  // Create a surface which is not attached to surface tree.
  static sptr<SurfaceControl> Create(const char* debug_name);

  static sptr<SurfaceControl> CreateFromWindow(NativeWindow* window,
                                               const char* debug_name);

  // Increase a ref for a surface
  void Acquire();

  // Decrease a ref for a surface
  void Release();

  void SetParent(SurfaceControl* new_parent);
  void SetVisibility(bool visibility);
  void SetZOrder(int32_t z_order);
  void SetBuffer(sptr<SurfaceBuffer> buffer,
                 UniqueFd fence_fd,
                 const BufferReleaseCallback& callback);
  void SetCrop(const Rect* crop);
  void SetPosition(int32_t x, int32_t y);
  void SetBufferTransform(int32_t transform);
  void SetScale(float x_scale, float y_scale);
  void SetBufferTransparency(int32_t transparency);
  void SetDamageRegion(std::vector<Rect> rects);
  void SetDesiredPresentTime(int64_t desired_present_time);
  void SetBufferAlpha(float alpha);
  void SetFrameRateWithChangeStrategy(float frame_rate,
                                      int8_t compatibility,
                                      int32_t strategy);
  void ClearFrameRate();
  void SetEnableBackPressure(bool enable_back_pressure);

  void SyncBufferToNodeIfNecessary();

 private:
  SurfaceControl(const SurfaceControl&) = delete;
  SurfaceControl& operator=(const SurfaceControl&) = delete;
  SurfaceControl(SurfaceControl&&) = delete;
  SurfaceControl& operator=(SurfaceControl&&) = delete;

  std::shared_ptr<OHOS::Rosen::RSSurfaceNode> surface_node_;
  std::shared_ptr<OHOS::Rosen::RSNode> parent_node_;
  sptr<SurfaceBuffer> buffer_;
  UniqueFd fence_fd_;
  BufferReleaseCallback release_callback_;
  std::vector<Rect> damage_region_;
  GraphicTransformType buffer_transform_ = GRAPHIC_ROTATE_NONE;
  int64_t desired_present_time_ = 0;
  bool need_sync_buffer_to_node_ = false;
};
;

}  // namespace OHOS::surface_control

#endif  // SURFACE_CONTROL_SURFACE_CONTROL_H_
