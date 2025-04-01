#include "surface_control/surface_control_render_node.h"

#include "base/log/dump_log.h"
#include "render_service_client/core/ui/rs_surface_node.h"
#include "surface_control/surface_control_component.h"
#include "surface_control/log.h"

#include <list>

namespace OHOS::Ace {

SurfaceControlRenderNode::SurfaceControlRenderNode() {
  Initialize();
}

SurfaceControlRenderNode::~SurfaceControlRenderNode() = default;

// static
RefPtr<RenderNode> SurfaceControlRenderNode::Create() {
  return AceType::MakeRefPtr<SurfaceControlRenderNode>();
}

void SurfaceControlRenderNode::Initialize() {}

void SurfaceControlRenderNode::Update(const RefPtr<Component>& component) {
  const RefPtr<SurfaceControlComponent> surfaceControl =
      AceType::DynamicCast<SurfaceControlComponent>(component);
  if (!surfaceControl) {
    return;
  }

  // textureId_ = surfaceControl->GetTextureId();

  // const auto& taskPool = surfaceControl->GetTaskPool();
  // if (taskPool) {
  //   taskPool->SetRenderNode(AceType::WeakClaim(this));
  //   pool_ = taskPool;
  //   tasks_ = std::list<TaskFunction>(taskPool->GetTasks().begin(),
  //                                    taskPool->GetTasks().end());
  //   taskPool->ClearTasks();
  //   pool_->SetPushToRenderNodeFunc(
  //       [weak = AceType::WeakClaim(this)](const TaskFunction& taskFunc) {
  //         auto client = weak.Upgrade();
  //         if (client) {
  //           client->PushTask(taskFunc);
  //         }
  //       });
  // }

  // MarkNeedLayout();
}

void SurfaceControlRenderNode::SyncGeometryProperties() {
  LOGE("EEEE SurfaceControlRenderNode::SyncGeometryProperties()");
  //     if (!IsTailRenderNode()) {
  //         return;
  //     }
  //     auto rsNode = GetRSNode();
  //     if (!rsNode) {
  //         return;
  //     }
  //     Offset paintOffset = GetPaintOffset();
  //     rsNode->SetBounds(paintOffset.GetX(), paintOffset.GetY(),
  //         drawSize_.Width(), drawSize_.Height());
  //
}

void SurfaceControlRenderNode::PerformLayout() {
  LOGE("EEEE SurfaceControlRenderNode::PerformLayout()");
  // if (!NeedLayout()) {
  //   return;
  // }

  // // render surfaceControl do not support child.
  // drawSize_ =
  //     Size(GetLayoutParam().GetMaxSize().Width(),
  //          (GetLayoutParam().GetMaxSize().Height() == Size::INFINITE_SIZE)
  //              ? Size::INFINITE_SIZE
  //              : (GetLayoutParam().GetMaxSize().Height()));
  // SetLayoutSize(drawSize_);
  // SetNeedLayout(false);
  // MarkNeedRender();
}

void SurfaceControlRenderNode::Paint(RenderContext& context,
                                     const Offset& offset) {
  LOGE("EEEE SurfaceControlRenderNode::Paint()");
  // position_ = GetGlobalOffset();
  // if (!isSurfaceInit_) {
  //   prePosition_ = position_;
  //   preDrawSize_ = drawSize_;

  //   NativeSurfaceControlOffset(position_.GetX(), position_.GetY());

  //   // The first time enter the Paint(), drawSize is (0, 0)
  //   // If the width or height equal to zero, it will not
  //   if (NearEqual(drawSize_.Width(), 0) || NearEqual(drawSize_.Height(), 0))
  //   {
  //     RenderNode::Paint(context, offset);
  //     return;
  //   }

  //   if (surfaceControlSizeInitEvent_ && (!drawSize_.IsHeightInfinite())) {
  //     surfaceControlSizeInitEvent_(textureId_, drawSize_.Width(),
  //                              drawSize_.Height());
  //     // Save the size in case it changes before OnSurfaceCreated() is called
  //     initDrawSize_ = drawSize_;
  //     isSurfaceInit_ = true;
  //   }
  // } else {
  //   if ((!NearEqual(prePosition_.GetX(), position_.GetX())) ||
  //       (!NearEqual(prePosition_.GetY(), position_.GetY()))) {
  //     prePosition_ = position_;
  //     positionChange_ = true;
  //   }

  //   if ((!NearEqual(preDrawSize_.Width(), drawSize_.Width())) ||
  //       (!NearEqual(preDrawSize_.Height(), drawSize_.Height()))) {
  //     preDrawSize_ = drawSize_;
  //     sizeChange_ = true;
  //   }
  // }

  // if (positionChange_) {
  //   positionChange_ = false;
  //   NativeSurfaceControlOffset(position_.GetX(), position_.GetY());
  // }

  // if (sizeChange_) {
  //   sizeChange_ = false;
  //   if (surfaceControlSizeChangeEvent_ && (!drawSize_.IsHeightInfinite())) {
  //     surfaceControlSizeChangeEvent_(textureId_, drawSize_.Width(),
  //                                drawSize_.Height());
  //   }
  // }

  RenderNode::Paint(context, offset);
}

std::shared_ptr<RSNode> SurfaceControlRenderNode::CreateRSNode() const {
  LOGE("EEEE SurfaceControlRenderNode::CreateRSNode()");
  std::string surfaceNodeName = std::string("SurfaceControlRenderNode") + id_;
  struct Rosen::RSSurfaceNodeConfig surfaceNodeConfig = {.SurfaceNodeName =
                                                             surfaceNodeName};
  return OHOS::Rosen::RSSurfaceNode::Create(surfaceNodeConfig, false);
}

void SurfaceControlRenderNode::DumpTree(int32_t depth) {
  auto children = GetChildren();

  if (DumpLog::GetInstance().GetDumpFile() > 0) {
    // DumpLog::GetInstance().AddDesc("textureId:", textureId_);
    // DumpLog::GetInstance().AddDesc("drawSize:", "width = ",
    // drawSize_.Width(), " height = ", drawSize_.Height());
    // DumpLog::GetInstance().AddDesc("sourceSize:", "width = ",
    // drawSize_.Width(), " height = ", drawSize_.Height());
    // DumpLog::GetInstance().Print(depth, AceType::TypeName(this),
    // children.size());
  }

  for (const auto& item : children) {
    item->DumpTree(depth + 1);
    // std::list<int, int, int> a;
  }
}

}  // namespace OHOS::Ace
