#ifndef HELLO_CONSTANTS_H_
#define HELLO_CONSTANTS_H_

namespace hello {

extern const char kPictureSkyUri[];
extern const char kPictureRiverUri[];
extern const char kVideoURL[];

constexpr int kFrameRate = 60;

constexpr float kScaleFactor = 3.25f;

constexpr int kEGLSurfaceNodeX = 16;
constexpr int kEGLSurfaceNodeY = 16;
constexpr int kEGLSurfaceNodeSize = 360;

constexpr int kBitmapNodeX = 38;
constexpr int kBitmapNodeY = 340;
constexpr int kBitmapNodeSize = 320;

}  // namespace hello

#endif
