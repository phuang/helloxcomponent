//
// Created on 2025-03-08.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "hello/BitmapRenderer.h"

#include <multimedia/image_framework/image/image_source_native.h>

#include <algorithm>
#include <cstdlib>

#include "common/log.h"

namespace hello {

BitmapRenderer::BitmapRenderer(const std::string &uri) {
  dx_ = std::abs(rand()) % 20 - 10;
  dy_ = std::abs(rand()) % 20 - 10;
  x_ = std::abs(std::rand()) % (picture_width_);
  y_ = std::abs(std::rand()) % (picture_height_);
  LoadPicture(uri);
}

void BitmapRenderer::LoadPicture(const std::string &uri) {
  OH_ImageSourceNative *image_source = nullptr;
  OH_PixelmapNative *pixelmap = nullptr;
  OH_Pixelmap_ImageInfo *image_info = nullptr;

  do {
    Image_ErrorCode retval =
        OH_ImageSourceNative_CreateFromUri(const_cast<char *>(uri.c_str()), uri.size(), &image_source);
    if (retval != IMAGE_SUCCESS) {
      LOGE("EEEE OH_ImageSourceNative_CreateFromUri() failed retval=%{public}d", retval);
      break;
    }

    retval = OH_ImageSourceNative_CreatePixelmap(image_source, nullptr, &pixelmap);
    if (retval != IMAGE_SUCCESS) {
      LOGE("EEEE OH_ImageSourceNative_CreatePixelmap() failed retval=%{public}d", retval);
      break;
    }

    retval = OH_PixelmapImageInfo_Create(&image_info);
    if (retval != IMAGE_SUCCESS) {
      LOGE("EEEE OH_PixelmapImageInfo_Create() failed retval=%{public}d", retval);
      break;
    }

    retval = OH_PixelmapNative_GetImageInfo(pixelmap, image_info);
    if (retval != IMAGE_SUCCESS) {
      LOGE("EEEE OH_PixelmapNative_GetImageInfo() failed retval=%{public}d", retval);
      break;
    }

    int32_t format = 0;
    OH_PixelmapImageInfo_GetWidth(image_info, &picture_width_);
    OH_PixelmapImageInfo_GetHeight(image_info, &picture_height_);
    OH_PixelmapImageInfo_GetRowStride(image_info, &picture_stride_);
    OH_PixelmapImageInfo_GetPixelFormat(image_info, &format);

    LOGE("EEEE size=%{public}dx%{public}d stride=%{public}d format=%{public}d", picture_width_, picture_height_,
         picture_stride_, format);

    picture_pixels_.resize(picture_height_ * picture_stride_);

    size_t buffer_size = picture_pixels_.size();
    retval = OH_PixelmapNative_ReadPixels(pixelmap, picture_pixels_.data(), &buffer_size);
    if (retval != IMAGE_SUCCESS) {
      LOGE("EEEE OH_PixelmapNative_ReadPixels() failed retval=%{public}d", retval);
      break;
    }
  } while (false);

  if (image_info)
    OH_PixelmapImageInfo_Release(image_info);
  if (pixelmap)
    OH_PixelmapNative_Release(pixelmap);
  if (image_source)
    OH_ImageSourceNative_Release(image_source);
}

void BitmapRenderer::Render(void *pixels, int32_t width, int32_t height, int32_t stride, uint64_t timestamp) {
  x_ += dx_;
  y_ += dy_;

  if (x_ < 0) {
    dx_ = -dx_;
    x_ = 0;
  }

  if (y_ < 0) {
    dy_ = -dy_;
    y_ = 0;
  }

  if (x_ + width > picture_width_) {
    dx_ = -std::abs(dx_);
    x_ = picture_width_ - width;
  }

  if (y_ + height > picture_height_) {
    dy_ = -std::abs(dy_);
    y_ = picture_height_ - height;
  }

  // Copy pixels
  uint8_t *dst = reinterpret_cast<uint8_t *>(pixels);
  uint8_t *src = picture_pixels_.data() + y_ * picture_stride_ + x_ * 4;
  size_t memcpy_size = std::min(static_cast<size_t>(width), static_cast<size_t>(picture_width_ - x_)) * 4;
  size_t copy_rows = std::min(static_cast<size_t>(height), static_cast<size_t>(picture_height_ - y_));
  for (; copy_rows > 0; --copy_rows) {
    memcpy(dst, src, memcpy_size);
    dst += stride;
    src += picture_stride_;
  }
}

} // namespace hello