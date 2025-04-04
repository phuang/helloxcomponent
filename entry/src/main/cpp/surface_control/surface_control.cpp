#include "surface_control/surface_control.h"

#include "foundation/graphic/graphic_2d/rosen/modules/render_service_client/core/pipeline/rs_node_map.h"
#include "foundation/graphic/graphic_2d/rosen/modules/render_service_client/core/ui/rs_proxy_node.h"
#include "foundation/graphic/graphic_2d/rosen/modules/render_service_client/core/ui/rs_surface_node.h"
#include "foundation/graphic/graphic_surface/interfaces/inner_api/surface/surface_buffer.h"
#include "foundation/graphic/graphic_surface/surface/include/native_window.h"
#include "surface_control/log.h"
#include "surface_control/ndk/surface_control.h"

namespace OHOS::surface_control {
namespace {
using OHOS::Rosen::RSNode;
using OHOS::Rosen::RSNodeMap;
using OHOS::Rosen::RSProxyNode;
using OHOS::Rosen::RSSurfaceNode;
using OHOS::Rosen::RSSurfaceNodeConfig;

std::shared_ptr<RSSurfaceNode> CreateSurfaceNode(const char* debug_name) {
  RSSurfaceNodeConfig config = {
      .SurfaceNodeName = debug_name,
  };
  auto node = RSSurfaceNode::Create(config, /*isWindow=*/false,
                                    /*rsUIContext=*/nullptr);
  return node;
}

}  // namespace

sptr<SurfaceControl> SurfaceControl::Create(const char* debug_name) {
  return sptr<SurfaceControl>::MakeSptr(CreateSurfaceNode(debug_name));
}

sptr<SurfaceControl> SurfaceControl::CreateFromWindow(NativeWindow* window,
                                                      const char* debug_name) {
  if (window->nodeId == 0) {
    return {};
  }

  auto parent_node = RSNodeMap::Instance().GetNode(window->nodeId);
  if (!parent_node) {
    parent_node = RSProxyNode::Create(window->nodeId, "root_proxy");
  }

  if (!parent_node) {
    LOGE("RSProxyNode::Create() failed: nodeId=%{public}lu", window->nodeId);
    return {};
  }

  auto surface_node = CreateSurfaceNode(debug_name);
  // RSProxyNode::AddChild() doesn't allow add child, has to call
  // RSNode::AddChild() instead.
  parent_node->RSNode::AddChild(surface_node, -1);
  return sptr<SurfaceControl>::MakeSptr(std::move(surface_node),
                                        std::move(parent_node));
}

SurfaceControl::SurfaceControl(std::shared_ptr<RSSurfaceNode> surface_node,
                               std::shared_ptr<RSNode> parent_node)
    : surface_node_(std::move(surface_node)),
      parent_node_(std::move(parent_node)) {}

SurfaceControl::~SurfaceControl() {
  // RSProxyNode::RemoveChild() does nothing, has to call RSNode::RemoveChild()
  // instead.
  if (parent_node_) {
    parent_node_->RSNode::RemoveChild(surface_node_);
  }
}

void SurfaceControl::Acquire() {
  RefBase::IncStrongRef(this);
}

void SurfaceControl::Release() {
  RefBase::DecStrongRef(this);
}

void SurfaceControl::SetParent(SurfaceControl* new_parent) {
  FATAL_IF(parent_node_, "SetParent() cannot be called with root surface");
  new_parent->surface_node_->AddChild(surface_node_, /*index=*/-1);
}

void SurfaceControl::SetVisibility(bool visibility) {
  surface_node_->SetVisible(visibility);
}

void SurfaceControl::SetZOrder(int32_t z_order) {
  if (auto parent = surface_node_->GetParent()) {
    parent->MoveChild(surface_node_, z_order);
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
  LOGE("EEEE SetBuffer() buffer=%{public}p, fence_fd=%{public}d",
       buffer_.GetRefPtr(), fence_fd_.Get());
}

void SurfaceControl::SetCrop(const Rect* crop) {
  surface_node_->SetFrame(crop->x, crop->y, crop->w, crop->h);
  // surface_node_->SetBounds(crop->x, crop->y, crop->w, crop->h);
}

void SurfaceControl::SetPosition(int32_t x, int32_t y) {
  surface_node_->SetTranslate(static_cast<float>(x), static_cast<float>(y), 0);
}

void SurfaceControl::SetBufferTransform(int32_t transform) {
  if (buffer_transform_ != static_cast<GraphicTransformType>(transform)) {
    buffer_transform_ = static_cast<GraphicTransformType>(transform);
    need_sync_buffer_to_node_ = true;
  }
}

void SurfaceControl::SetScale(float scale_x, float scale_y) {
  surface_node_->SetScale(scale_x, scale_y);
}

void SurfaceControl::SetRotation(float degree_x,
                                 float degree_y,
                                 float degree_z) {
  surface_node_->SetRotation(degree_x, degree_y, degree_z);
}

void SurfaceControl::SetBufferTransparency(int32_t transparency) {}

void SurfaceControl::SetDamageRegion(std::vector<Rect> rects) {
  damage_region_ = std::move(rects);
}

void SurfaceControl::SetDesiredPresentTime(int64_t desired_present_time) {
  desired_present_time_ = desired_present_time;
}

void SurfaceControl::SetBufferAlpha(float alpha) {
  surface_node_->SetAlpha(alpha);
}

void SurfaceControl::SetFrameRateWithChangeStrategy(float frame_rate,
                                                    int8_t compatibility,
                                                    int32_t strategy) {
  LOGE("SetFrameRateWithChangeStrategy() is not implemented");
}

void SurfaceControl::ClearFrameRate() {
  LOGE("ClearFrameRate() is not implemented");
}

void SurfaceControl::SetEnableBackPressure(bool enable_back_pressure) {
  LOGE("SetEnableBackPressure() is not implemented");
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
    surface_node_->SetBuffer(
        std::move(params),
        [cb = release_callback_](sptr<SyncFence> fence) { cb(fence->Get()); });
    need_sync_buffer_to_node_ = false;
    LOGE("SyncBufferToNodeIfNecessary() buffer=%{public}p, fence_fd=%{public}d",
         buffer_.GetRefPtr(), fence_fd_.Get());
  }
}

}  // namespace OHOS::surface_control
