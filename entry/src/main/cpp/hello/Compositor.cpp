#include "hello/Compositor.h"

#include "hello/BitmapRenderer.h"
#include "hello/Constants.h"
#include "hello/TextureRenderer.h"

namespace hello {

Compositor::Compositor() {
  texture_renderer_ = std::make_unique<TextureRenderer>();
  bitmap_renderer_ = std::make_unique<BitmapRenderer>(kChildPictureUri);
}

Compositor::~Compositor() {}

void Compositor::RenderFrame(int32_t width,
                             int32_t height,
                             uint64_t timestamp) {}
}  // namespace hello
