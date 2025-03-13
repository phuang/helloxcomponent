#include "hello/Compositor.h"

#include <EGL/egl.h>
#include <GLES3/gl3.h>

#include <mutex>

#include "hello/BitmapRenderer.h"
#include "hello/Constants.h"
#include "hello/GLCore.h"
#include "hello/Log.h"
#include "hello/Matrix.h"
#include "hello/NativeWindow.h"
#include "hello/SyncFence.h"
#include "hello/TextureRenderer.h"

namespace hello {
namespace {

enum Mode {
  // Upload data with glTexImage2D()
  kTexture,
  // Create NativeImage and NativeWindows which use buffer queue.
  // At producer side,
  // OH_NativeWindow_NativeWindowRequestBuffer() and
  // OH_NativeWindow_NativeWindowFlushBuffer() are used.
  // At consumer side, OH_NativeImage_AttachContext() and
  // OH_NativeImage_UpdateSurfaceImage() are used.
  kNativeWindow,
};

const Mode kMode = kNativeWindow;

const int32_t kWindowWidth = 1260;
const int32_t kWindowHeight = 2530;
const int kPictureSize = 1040;
const float kRootScale = 1.0f;

// clang-format off
const GLfloat kQuadVertices[] = {
  // Positions    // Texture Coords
  -1.0f,  1.0f,   0.0f, 0.0f,
  -1.0f, -1.0f,   0.0f, 1.0f,
   1.0f, -1.0f,   1.0f, 1.0f,
   1.0f,  1.0f,   1.0f, 0.0f,
};

const GLuint kQuadIndices[] = {
  0, 1, 2,
  2, 3, 0,
};
// clang-format on

constexpr int kStride = 4 * sizeof(GLfloat);
constexpr int kPositionOffset = 0;
constexpr int kTexCoordOffset = 2 * sizeof(GLfloat);

const char kVertexShaderSource[] = R"(#version 300 es
layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_texCoord;
uniform mat4 u_transform;
out vec2 v_texCoord;

void main() {
  gl_Position = u_transform * vec4(a_position, 0.0, 1.0);
  v_texCoord = a_texCoord;
}
)";

const char kFragmentShaderSource[] = R"(#version 300 es
precision mediump float;
in vec2 v_texCoord;
uniform sampler2D u_texture;
out vec4 fragColor;

void main() {
  fragColor = texture(u_texture, v_texCoord);
}
)";

const char kFragmentShaderEGLImageSource[] = R"(#version 300 es
#extension GL_OES_EGL_image_external_essl3 : require
precision mediump float;
in vec2 v_texCoord;
uniform samplerExternalOES u_texture;
out vec4 fragColor;

void main() {
  fragColor = texture(u_texture, v_texCoord);
}
)";

GLuint program_ = 0;
GLuint vao_ = 0;
GLuint vbo_ = 0;
GLuint ebo_ = 0;

void SetupGL() {
  static std::once_flag flag;

  std::call_once(flag, [] {
    // Create VAO, VBO, and EBO
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kQuadVertices), kQuadVertices,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, kStride,
                          (GLvoid*)kPositionOffset);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, kStride,
                          (GLvoid*)kTexCoordOffset);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kQuadIndices), kQuadIndices,
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    program_ = GLCore::CreateProgram(
        kVertexShaderSource, kMode == kTexture ? kFragmentShaderSource
                                               : kFragmentShaderEGLImageSource);
  });
}

}  // namespace

Compositor::Compositor() {
  SetupGL();
  renderers_[0] = std::make_unique<BitmapRenderer>(kPictureSkyUri);
  renderers_[1] = std::make_unique<BitmapRenderer>(kPictureRiverUri);

  switch (kMode) {
    case kTexture: {
      textures_[0] = GLTexture::Create();
      textures_[1] = GLTexture::Create();
      break;
    }
    case kNativeWindow: {
      native_windows_[0] = NativeWindow::Create(kWindowWidth, kWindowHeight);
      native_windows_[1] = NativeWindow::Create(kPictureSize, kPictureSize);
      textures_[0] = native_windows_[0]->BindTexture();
      textures_[1] = native_windows_[1]->BindTexture();
      break;
    }
  }
}

Compositor::~Compositor() = default;

void Compositor::RenderFrame(int32_t width,
                             int32_t height,
                             uint64_t timestamp) {
  switch (kMode) {
    case kTexture: {
      UploadTextures(width, height, timestamp);
      RenderFrameWithTexture(width, height, timestamp);
      break;
    }
    case kNativeWindow: {
      UploadNativeWindows(width, height, timestamp);
      RenderFrameWithTexture(width, height, timestamp);
      break;
    }
  }
}

void Compositor::UploadNativeWindows(int32_t width,
                                     int32_t height,
                                     uint64_t timestamp) {
  for (int i = 0; i < 2; ++i) {
    int32_t w, h, stride;
    ScopedFd fence_fd;
    void* addr = nullptr;
    native_windows_[i]->RequestBuffer(&w, &h, &stride, &fence_fd, &addr);
    SyncFence sync_fence(std::move(fence_fd));
    sync_fence.Wait(-1);
    renderers_[i]->RenderPixels(static_cast<uint8_t*>(addr), w, h, stride, 0);
    native_windows_[i]->FlushBuffer();
    native_windows_[i]->UpdateSurfaceImage();
  }
}

void Compositor::UploadTextures(int32_t width,
                                int32_t height,
                                uint64_t timestamp) {
  GLenum target;
  auto texture_upload = [&target](const uint8_t* src, uint32_t copy_width,
                                  uint32_t copy_height, uint32_t src_stride) {
    glPixelStorei(GL_UNPACK_ROW_LENGTH, src_stride / 4);
    glTexImage2D(target, 0, GL_RGBA, copy_width, copy_height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, src);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  };

  target = textures_[0].target();
  glBindTexture(textures_[0].target(), textures_[0].id());
  renderers_[0]->RenderPixels(kWindowWidth, kWindowHeight, timestamp,
                              texture_upload);

  target = textures_[0].target();
  glBindTexture(textures_[0].target(), textures_[1].id());
  renderers_[1]->RenderPixels(kPictureSize, kPictureSize, timestamp,
                              texture_upload);
}

void Compositor::RenderFrameWithTexture(int32_t width,
                                        int32_t height,
                                        uint64_t timestamp) {
  // Set the viewport
  glViewport(0, 0, width, height);

  glClearColor(0.8, 0.8, 0.8, 1.0f);
  glClearDepthf(1.0f);
  glDisable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT);

  // Use the shader program
  glUseProgram(program_);
  // set cull
  glEnable(GL_CULL_FACE);

  // Draw the cube
  glBindVertexArray(vao_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);

  {
    // Rotate the triangle by angle{X,Y,Z}
    Matrix4x4 transfrom_matrix = Matrix4x4::Scale(kRootScale, kRootScale, 1.0f);

    GLint u_transform_location = glGetUniformLocation(program_, "u_transform");
    glUniformMatrix4fv(u_transform_location, 1, GL_FALSE,
                       transfrom_matrix.data);

    // Bind texture_1 to texture unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(textures_[0].target(), textures_[0].id());
    GLint u_texture_location = glGetUniformLocation(program_, "u_texture");
    glUniform1i(u_texture_location, 0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  }

  {
    Matrix4x4 transfrom_matrix = Matrix4x4::Scale(
        1.0f * kPictureSize / width, 1.0f * kPictureSize / height, 1);

    GLint u_transform_location = glGetUniformLocation(program_, "u_transform");
    glUniformMatrix4fv(u_transform_location, 1, GL_FALSE,
                       transfrom_matrix.data);

    // Bind texture_1 to texture unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(textures_[1].target(), textures_[1].id());
    GLint u_texture_location = glGetUniformLocation(program_, "u_texture");
    glUniform1i(u_texture_location, 0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  }

  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

}  // namespace hello
