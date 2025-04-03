#include "surface_control/surface_control.h"

#include <parcel.h>

#include "foundation/graphic/graphic_2d/rosen/modules/render_service_client/core/ui/rs_surface_node.h"
#include "foundation/graphic/graphic_surface/interfaces/inner_api/surface/surface_buffer.h"
#include "surface_control/ndk/surface_control.h"

namespace OHOS::surface_control {
namespace {
using OHOS::Rosen::RSSurfaceNode;
using OHOS::Rosen::RSSurfaceNodeConfig;

class Helper : public RSSurfaceNode {
 public:
  static std::shared_ptr<RSSurfaceNode> CreateProxy(NativeWindow* window,
                                                    const char* debug_name) {
    RSSurfaceNodeConfig config = {
        .SurfaceNodeName = debug_name,
    };
    auto node = RSSurfaceNode::Create(config, /*isWindow=*/false,
                                      /*rsUIContext=*/nullptr);
    // node->skipDestroyCommandInDestructor_ = true;
    return node;
  }
};

}  // namespace

sptr<SurfaceControl> SurfaceControl::Create(const char* debug_name) {
  RSSurfaceNodeConfig config = {
      .SurfaceNodeName = debug_name,
  };
  auto node = RSSurfaceNode::Create(config, /*isWindow=*/false,
                                    /*rsUIContext=*/nullptr);
  return sptr<SurfaceControl>::MakeSptr(std::move(node));
}

sptr<SurfaceControl> SurfaceControl::CreateFromWindow(NativeWindow* window,
                                                      const char* debug_name) {
  Parcel parcel;
  uint64_t id = 0;
  parcel.WriteUint64(id);
  parcel.WriteString(debug_name);
  parcel.WriteBool(/*isRenderServiceNode=*/false);
  auto node = RSSurfaceNode::Unmarshalling(parcel);
  return sptr<SurfaceControl>::MakeSptr(std::move(node));
}

SurfaceControl::SurfaceControl(std::shared_ptr<RSSurfaceNode> node)
    : node_(std::move(node)) {}

SurfaceControl::~SurfaceControl() = default;

void SurfaceControl::Acquire() {
  RefBase::IncStrongRef(this);
}

void SurfaceControl::Release() {
  RefBase::DecStrongRef(this);
}

void SurfaceControl::SetParent(SurfaceControl* new_parent) {
  new_parent->node_->AddChild(node_, /*index=*/-1);
}

void SurfaceControl::SetVisibility(bool visibility) {
  node_->SetVisible(visibility);
}

void SurfaceControl::SetZOrder(int32_t z_order) {
  if (auto parent = node_->GetParent()) {
    parent->MoveChild(node_, z_order);
  }
}

void SurfaceControl::SetBuffer(sptr<SurfaceBuffer> buffer,
                               UniqueFd fence_fd,
                               const BufferReleaseCallback& callback) {
  if (buffer_ && release_callback_) {
    release_callback_(std::move(fence_fd_));
  }
  buffer_ = std::move(buffer);
  fence_fd_ = std::move(fence_fd);
  release_callback_ = callback;
  need_sync_buffer_to_node_ = true;
}

void SurfaceControl::SetCrop(const Rect* crop) {
  node_->SetFrame(crop->x, crop->y, crop->w, crop->h);
}

void SurfaceControl::SetPosition(int32_t x, int32_t y) {
  node_->SetTranslate(static_cast<float>(x), static_cast<float>(y), 0);
}

void SurfaceControl::SetBufferTransform(int32_t transform) {
  if (buffer_transform_ != static_cast<GraphicTransformType>(transform)) {
    buffer_transform_ = static_cast<GraphicTransformType>(transform);
    need_sync_buffer_to_node_ = true;
  }
}

void SurfaceControl::SetScale(float x_scale, float y_scale) {
  node_->SetScale(x_scale, y_scale);
}

void SurfaceControl::SetBufferTransparency(int32_t transparency) {}

void SurfaceControl::SetDamageRegion(std::vector<Rect> rects) {
  damage_region_ = std::move(rects);
}

void SurfaceControl::SetDesiredPresentTime(int64_t desired_present_time) {
  desired_present_time_ = desired_present_time;
}

void SurfaceControl::SetBufferAlpha(float alpha) {
  // Implementation to set the buffer alpha
}

void SurfaceControl::SetFrameRateWithChangeStrategy(float frame_rate,
                                                    int8_t compatibility,
                                                    int32_t strategy) {
  // Implementation to set the frame rate with a change strategy
}

void SurfaceControl::ClearFrameRate() {
  // Implementation to clear the frame rate
}

void SurfaceControl::SetEnableBackPressure(bool enable_back_pressure) {
  // Implementation to enable or disable back pressure
}

void SurfaceControl::SyncBufferToNodeIfNecessary() {
  if (need_sync_buffer_to_node_) {
    RSSurfaceNode::SetBufferParams params;
    buffer_->SetSurfaceBufferTransform(buffer_transform_);
    params.buffer = buffer_;
    params.fence = new SyncFence(fence_fd_.Release());
    params.damages = std::move(damage_region_);
    params.timestamp = desired_present_time_;
    // TODO: set HDR related fields.
    node_->SetBuffer(std::move(params), {});
    need_sync_buffer_to_node_ = false;
  }
}

}  // namespace OHOS::surface_control
