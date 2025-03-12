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
#include "hello/TextureRenderer.h"

namespace hello {
namespace {

// clang-format off
const GLfloat kQuadVertices[] = {
  // Positions    // Texture Coords
  -1.0f,  1.0f,   0.0f, 1.0f,
  -1.0f, -1.0f,   0.0f, 0.0f,
   1.0f, -1.0f,   1.0f, 0.0f,
   1.0f,  1.0f,   1.0f, 1.0f,
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
  gl_Position = u_transform * vec4(a_position, 1.0, 1.0);
  // gl_Position = vec4(a_position, 1.0, 1.0);
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
  // fragColor = vec4(1.0, 0.0, 0.0, 1.0);
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

    program_ =
        GLCore::CreateProgram(kVertexShaderSource, kFragmentShaderSource);
  });
}

}  // namespace

Compositor::Compositor() {
  SetupGL();
  renderers_[0] = std::make_unique<BitmapRenderer>(kPictureSkyUri);
  renderers_[1] = std::make_unique<BitmapRenderer>(kPictureRiverUri);
}

Compositor::~Compositor() = default;

void Compositor::RenderFrame(int32_t width,
                             int32_t height,
                             uint64_t timestamp) {
  // LOGE("EEEE Compositor::RenderFrame() %{public}d", __LINE__);

  auto texture_upload = [](const uint8_t* src, uint32_t copy_width,
                           uint32_t copy_height, uint32_t src_stride) {
    LOGE("EEEEE texture_upload() %{public}d x %{public}d src=%{public}p",
         copy_width, copy_height, src);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, src_stride / 4);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, copy_width, copy_height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, src);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    CHECK_GL_ERROR();
  };

  GLuint textures[2] = {0, 0};
  glGenBuffers(2, textures);

  glBindTexture(GL_TEXTURE_2D, textures[0]);
  renderers_[0]->RenderPixels(width, height, timestamp, texture_upload);

  glBindTexture(GL_TEXTURE_2D, textures[1]);
  renderers_[1]->RenderPixels(kBitmapNodeSize, kBitmapNodeSize, timestamp,
                              texture_upload);

  // // Create and bind the texture
  // GLuint red_texture;
  // glGenTextures(1, &red_texture);
  // glBindTexture(GL_TEXTURE_2D, red_texture);

  // // Set texture parameters
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // // Upload the texture data
  // std::vector<uint8_t> red_texture_data(width * height * 4 + 4, 255);
  // for (size_t i = 0; i < red_texture_data.size(); i += 4) {
  //   red_texture_data[i] = 255;      // Red
  //   red_texture_data[i + 1] = i;    // Green
  //   red_texture_data[i + 2] = 0;    // Blue
  //   red_texture_data[i + 3] = 255;  // Alpha
  // }
  // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
  //              GL_UNSIGNED_BYTE, red_texture_data.data() + 2);
  // CHECK_GL_ERROR();

  // Set the viewport
  glViewport(0, 0, width, height);

  glClearColor(1.0, 1.0, 1.0, 1.0f);
  glClearDepthf(1.0f);
  glDisable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Use the shader program
  glUseProgram(program_);

  // Rotate the triangle by angle{X,Y,Z}
  Matrix4x4 transfrom_matrix = Matrix4x4::Scale(0.5, 0.5, 1);

  GLint u_transform_location = glGetUniformLocation(program_, "u_transform");
  glUniformMatrix4fv(u_transform_location, 1, GL_FALSE, transfrom_matrix.data);
  CHECK_GL_ERROR();

  // Bind texture_1 to texture unit 0
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textures[1]);
  // glBindTexture(GL_TEXTURE_2D, red_texture);
  GLint u_texture_location = glGetUniformLocation(program_, "u_texture");
  glUniform1i(u_texture_location, 0);
  CHECK_GL_ERROR();

  // set cull
  glEnable(GL_CULL_FACE);
  // glCullFace(GL_BACK);

  // Draw the cube
  glBindVertexArray(vao_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glDeleteTextures(2, textures);
  // glDeleteTextures(1, &red_texture);

  CHECK_GL_ERROR();
}

}  // namespace hello
