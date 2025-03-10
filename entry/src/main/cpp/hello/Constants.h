#ifndef HELLO_CONSTANTS_H_
#define HELLO_CONSTANTS_H_

namespace hello {

extern const char kRootPictureUri[];
extern const char kChildPictureUri[];

// Mate 60 default scale factor

constexpr float kScaleFactor = 3.25f;

constexpr int kEGLSurfaceNodeX = 16;
constexpr int kEGLSurfaceNodeY = 16;
constexpr int kEGLSurfaceNodeSize = 360;

constexpr int kBitmapNodeX = 36;
constexpr int kBitmapNodeY = 340;
constexpr int kBitmapNodeSize = 320;

}  // namespace hello

#endif
