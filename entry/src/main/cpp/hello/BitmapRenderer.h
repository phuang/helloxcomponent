//
// Created on 2025-03-08.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#ifndef HELLOXCOMPONENT_BITMAPRENDERER_H
#define HELLOXCOMPONENT_BITMAPRENDERER_H

#include <cstdint>
#include <string>
#include <vector>

#include "hello/XComponentNode.h"

namespace hello {

class BitmapRenderer : public XComponentNode::Delegate {
 public:
  explicit BitmapRenderer(const std::string& uri);
  ~BitmapRenderer() override = default;
  void RenderPixels(void* pixels,
                    int32_t width,
                    int32_t height,
                    int32_t stride,
                    uint64_t timestamp) override;

  const uint8_t* picture_pixels() const { return picture_pixels_.data(); }
  uint32_t picture_width() const { return picture_width_; }
  uint32_t picture_height() const { return picture_height_; }
  uint32_t picture_stride() const { return picture_stride_; }

 private:
  void LoadPicture(const std::string& uri);

  std::vector<uint8_t> picture_pixels_;
  uint32_t picture_width_ = 0;
  uint32_t picture_height_ = 0;
  uint32_t picture_stride_ = 0;
  int32_t x_ = 0;
  int32_t y_ = 0;
  int32_t dx_ = 0;
  int32_t dy_ = 0;
};

}  // namespace hello

#endif  // HELLOXCOMPONENT_BITMAPRENDERER_H
