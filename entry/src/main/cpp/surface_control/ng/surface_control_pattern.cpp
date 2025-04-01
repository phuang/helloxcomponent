#include "surface_control/ng/surface_control_pattern.h"

#include "core/components_ng/render/render_surface.h"
#include "core/pipeline_ng/pipeline_context.h"
#include "surface_control/log.h"
#include "surface_control/ng/surface_control_layout_algorithm.h"
#include "surface_control/ng/surface_control_layout_property.h"
#include "surface_control/ng/surface_control_paint_method.h"

namespace OHOS::Ace::NG {

SurfaceControlPattern::SurfaceControlPattern() {
  LOGE("EEEE SurfaceControlPattern::SurfaceControlPattern()");
}

SurfaceControlPattern::~SurfaceControlPattern() {
  LOGE("EEEE SurfaceControlPattern::~SurfaceControlPattern()");
}

bool SurfaceControlPattern::IsAtomicNode() const {
  // TODO: figure out what is atomic node
  LOGE("EEEE SurfaceControlPattern::IsAtomicNode()");
  return true;
}

RefPtr<LayoutProperty> SurfaceControlPattern::CreateLayoutProperty() {
  LOGE("EEEE SurfaceControlPattern::CreateLayoutProperty()");
  return MakeRefPtr<SurfaceControlLayoutProperty>();
}

RefPtr<EventHub> SurfaceControlPattern::CreateEventHub() {
  LOGE("EEEE SurfaceControlPattern::CreateEventHub()");
  // return MakeRefPtr<XComponentEventHub>();
  return nullptr;
}

RefPtr<LayoutAlgorithm> SurfaceControlPattern::CreateLayoutAlgorithm() {
  LOGE("EEEE SurfaceControlPattern::CreateLayoutAlgorithm()");
  return MakeRefPtr<SurfaceControlLayoutAlgorithm>();
}

RefPtr<NodePaintMethod> SurfaceControlPattern::CreateNodePaintMethod() {
  LOGE("EEEE SurfaceControlPattern::CreateNodePaintMethod()");
  auto paint = MakeRefPtr<SurfaceControlPaintMethod>(AceType::Claim(this));
  return paint;
}

FocusPattern SurfaceControlPattern::GetFocusPattern() const {
  LOGE("EEEE SurfaceControlPattern::GetFocusPattern()");
  return {FocusType::DISABLE, true};
}

bool SurfaceControlPattern::NeedSoftKeyboard() const {
  LOGE("EEEE SurfaceControlPattern::NeedSoftKeyboard()");
  return false;
}

void SurfaceControlPattern::OnAttachToFrameNode() {
  LOGE("EEEE SurfaceControlPattern::OnAttachToFrameNode()");
  Initialize();
}

void SurfaceControlPattern::OnAttachToMainTree() {
  LOGE("EEEE SurfaceControlPattern::OnAttachToMainTree()");
}

void SurfaceControlPattern::BeforeSyncGeometryProperties(
    const DirtySwapConfig& config) {
  LOGE("EEEE SurfaceControlPattern::OnDetachFromMainTree()");
}

void SurfaceControlPattern::OnDetachFromMainTree() {
  LOGE("EEEE SurfaceControlPattern::OnDetachFromMainTree()");
}

void SurfaceControlPattern::OnDetachFromFrameNode(FrameNode* frameNode) {
  LOGE("EEEE SurfaceControlPattern::OnDetachFromFrameNode()");
}

void SurfaceControlPattern::OnRebuildFrame() {
  LOGE("EEEE SurfaceControlPattern::OnRebuildFrame()");
  if (!renderSurface_->IsSurfaceValid()) {
    return;
  }
  auto host = GetHost();
  CHECK_NULL_VOID(host);
  auto renderContext = host->GetRenderContext();
  CHECK_NULL_VOID(renderContext);
  CHECK_NULL_VOID(renderContext_);
  renderContext->AddChild(renderContext_, 0);
  // TODO: Is it needed?
  // SetSurfaceNodeToGraphic();
}

void SurfaceControlPattern::OnAreaChangedInner() {
  LOGE("EEEE SurfaceControlPattern::OnAreaChangedInner()");
#if !defined(RENDER_EXTRACT_SUPPORTED)
  if (SystemProperties::GetExtSurfaceEnabled()) {
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto geometryNode = host->GetGeometryNode();
    CHECK_NULL_VOID(geometryNode);
    auto contentSize = geometryNode->GetContentSize();
    auto contentOffset = geometryNode->GetContentOffset();
    auto transformRelativeOffset = host->GetTransformRelativeOffset();
    renderSurface_->SetExtSurfaceBounds(
        transformRelativeOffset.GetX() + contentOffset.GetX(),
        transformRelativeOffset.GetY() + contentOffset.GetY(),
        contentSize.Width(), contentSize.Height());
  }
#endif
}

void SurfaceControlPattern::OnWindowHide() {
  LOGE("EEEE SurfaceControlPattern::OnWindowHide()");
  // Add logic to handle window hide event
}

void SurfaceControlPattern::OnWindowShow() {
  LOGE("EEEE SurfaceControlPattern::OnWindowShow()");
  // Add logic to handle window show event
}

void SurfaceControlPattern::Initialize() {
  LOGE("EEEE SurfaceControlPattern::Initialize()");
  // Add initialization logic here
  InitializeRenderSurface();
}

void SurfaceControlPattern::InitializeRenderSurface() {
  LOGE("EEEE SurfaceControlPattern::InitializeRenderSurface()");
  auto host = GetHost();
  CHECK_NULL_VOID(host);
  auto renderContext = host->GetRenderContext();
  CHECK_NULL_VOID(renderContext);

  renderContext->SetClipToFrame(true);
  renderContext->SetClipToBounds(true);

#ifdef RENDER_EXTRACT_SUPPORTED
  // TODO: figure out the differenace between SURFACE and TEXTURE
  renderSurface_ =
      RenderSurface::Create(RenderSurface::RenderSurfaceType::SURFACE);
#else
  renderSurface_ = RenderSurface::Create();
#endif

  renderSurface_->SetInstanceId(GetHostInstanceId());
  renderSurface_->SetBufferUsage("SurfaceControl");

  constexpr bool kIsSurface = true;
  if (kIsSurface) {
    // TODO: Is RenderContext needed?
    InitializeRenderContext();
    if (!SystemProperties::GetExtSurfaceEnabled()) {
      renderSurface_->SetRenderContext(renderContext_);
    } else {
      //       auto pipelineContext = host->GetContextRefPtr();
      //       CHECK_NULL_VOID(pipelineContext);
      //       pipelineContext->AddOnAreaChangeNode(host->GetId());
      //       extSurfaceClient_ =
      //           MakeRefPtr<XComponentExtSurfaceCallbackClient>(WeakClaim(this));
      //       renderSurface_->SetExtSurfaceCallback(extSurfaceClient_);
      // #if defined(RENDER_EXTRACT_SUPPORTED)
      //       RegisterRenderContextCallBack();
      // #endif
    }
    // handlingSurfaceRenderContext_ = renderContextForSurface_;
  } else {
    renderSurface_->SetRenderContext(renderContext);
    renderSurface_->SetIsTexture(true);
  }

  renderSurface_->InitSurface();
  renderSurface_->UpdateSurfaceConfig();

  if (!kIsSurface) {
    renderSurface_->RegisterBufferCallback();
  }
}

void SurfaceControlPattern::InitializeRenderContext() {
  LOGE("EEEE SurfaceControlPattern::InitializeRenderContext()");
  renderContext_ = RenderContext::Create();
  // TODO: figure out the difference between SURFACE and TEXTURE
  constexpr auto kContextType = RenderContext::ContextType::HARDWARE_SURFACE;
  RenderContext::ContextParam param = {kContextType, id_ + "Surface",
                                       RenderContext::PatternType::DEFAULT};
  CHECK_NULL_VOID(renderContext_);
  renderContext_->InitContext(false, param);

  renderContext_->UpdateBackgroundColor(Color::BLACK);
}

}  // namespace OHOS::Ace::NG
