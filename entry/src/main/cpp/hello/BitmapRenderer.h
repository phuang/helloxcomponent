//
// Created on 2025-03-08.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef HELLOXCOMPONENT_BITMAPRENDERER_H
#define HELLOXCOMPONENT_BITMAPRENDERER_H

#include <cstdint>

namespace hello {

class BitmapRenderer {
public:
  BitmapRenderer();
  void Render(void *pixels, int32_t width, int32_t height, int32_t stride, uint64_t timestamp);

private:
  int32_t x_ = 0;
  int32_t y_ = 0;
  int32_t dx_ = 0;
  int32_t dy_ = 0;
};

} // namespace hello

#endif // HELLOXCOMPONENT_BITMAPRENDERER_H
