#include "hello/SurfaceControl.h"

#include "hello/Constants.h"
#include "hello/Log.h"
#include "hello/NativeBuffer.h"
#include "hello/NativeWindow.h"

namespace hello {

// static
std::unique_ptr<SurfaceControl> SurfaceControl::Create(NativeWindow* parent,
                                                       const char* name) {
  LOGE("EEEE SurfaceControl::Create() parent=%{public}p name=%{public}s",
       parent, name);
  if (auto* surface =
          OH_SurfaceControl_FromNativeWindow(parent->window(), name)) {
    return std::unique_ptr<SurfaceControl>(new SurfaceControl(surface));
  }
  return {};
}

// static
std::unique_ptr<SurfaceControl> SurfaceControl::Create(const char* name) {
  if (auto* surface = OH_SurfaceControl_Create(name)) {
    return std::unique_ptr<SurfaceControl>(new SurfaceControl(surface));
  }
  return {};
}

SurfaceControl::SurfaceControl(OH_SurfaceControl* surface) : surface_(surface) {
  buffer_ = NativeBuffer::Create(512, 512);

  void* addr = buffer_->Map();
  if (addr) {
    memset(addr, 0xFF, buffer_->width() * buffer_->height() * 4);
  }
  buffer_->Unmap();

  auto* txn = OH_SurfaceTransaction_Create();

  OH_SurfaceTransaction_SetBuffer(txn, surface_, buffer_->buffer(), -1, this,
                                  SurfaceControl::OnBufferReleaseStub);
  OH_Rect crop = {
      .x = 0,
      .y = 0,
      .w = buffer_->width(),
      .h = buffer_->height(),
  };
  OH_SurfaceTransaction_SetCrop(txn, surface_, &crop);
  OH_SurfaceTransaction_SetVisibility(txn, surface_,
                                      OH_SURFACE_TRANSACTION_VISIBILITY_SHOW);
  OH_SurfaceTransaction_Commit(txn);
  OH_SurfaceTransaction_Delete(txn);
}

SurfaceControl::~SurfaceControl() {
  if (surface_) {
    OH_SurfaceControl_Release(surface_);
  }
}

void SurfaceControl::OnBufferRelease(int release_fence_fd) {
  LOGE("EEEE Buffer released");
}

// static
void SurfaceControl::OnBufferReleaseStub(void* context, int release_fence_fd) {
  if (context) {
    auto* self = static_cast<SurfaceControl*>(context);
    self->OnBufferRelease(release_fence_fd);
  }
  if (release_fence_fd >= 0) {
    close(release_fence_fd);
  }
}
}  // namespace hello
