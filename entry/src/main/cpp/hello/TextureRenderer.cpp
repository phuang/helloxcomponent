#include "hello/TextureRenderer.h"

#include <mutex>

#include "hello/GLCore.h"
#include "hello/Log.h"
#include "hello/Matrix.h"

namespace hello {
namespace {

static const std::chrono::system_clock::time_point kStartTime =
    std::chrono::system_clock::now();

// clang-format off
const GLfloat kCubeVertices[] = {
  // float3 position, float4 color, float2 uv,
  1, -1, 1, 1, 0, 1, 1, 0, 1,
  -1, -1, 1, 0, 0, 1, 1, 1, 1,
  -1, -1, -1, 0, 0, 0, 1, 1, 0,
  1, -1, -1,  1, 0, 0, 1, 0, 0,

  1, 1, 1, 1, 1, 1, 1, 0, 1,
  1, -1, 1, 1, 0, 1, 1, 1, 1,
  1, -1, -1, 1, 0, 0, 1, 1, 0,
  1, 1, -1, 1, 1, 0, 1, 0, 0,

  -1, 1, 1, 0, 1, 1, 1, 0, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, -1, 1, 1, 0, 1, 1, 0,
  -1, 1, -1, 0, 1, 0, 1, 0, 0,

  -1, -1, 1, 0, 0, 1, 1, 0, 1,
  -1, 1, 1, 0, 1, 1, 1, 1, 1,
  -1, 1, -1, 0, 1, 0, 1, 1, 0,
  -1, -1, -1, 0, 0, 0, 1, 0, 0,

  1, 1, 1, 1, 1, 1, 1, 0, 1,
  -1, 1, 1, 0, 1, 1, 1, 1, 1,
  -1, -1, 1, 0, 0, 1, 1, 1, 0,
  1, -1, 1, 1, 0, 1, 1, 0, 0,

  1, -1, -1, 1, 0, 0, 1, 0, 1,
  -1, -1, -1, 0, 0, 0, 1, 1, 1,
  -1, 1, -1, 0, 1, 0, 1, 1, 0,
  1, 1, -1, 1, 1, 0, 1, 0, 0,
};

const GLuint kCubeIndices[] = {
  0,1,2,
  3,0,2,

  4,5,6,
  7,4,6,

  8,9,10,
  11,8,10,

  12,13,14,
  15,12,14,

  16,17,18,
  18,19,16,

  20,21,22,
  23,20,22,
};
// clang-format on

constexpr int kStride = 9 * sizeof(GLfloat);
constexpr int kPositionOffset = 0;
constexpr int kColorOffset = 3 * sizeof(GLfloat);
constexpr int kTexCoordOffset = 7 * sizeof(GLfloat);

// Shader sources (simple shaders to draw a triangle)
const char kVertexShaderSource[] = R"(#version 300 es
in vec3 aPosition;
in vec4 aColor;
out vec4 vColor;
in vec2 aTexCoord;
out vec2 vTexCoord;
uniform mat4 uRotationMatrix;
void main() {
  gl_Position = uRotationMatrix * vec4(aPosition, 1);
  vColor = aColor;
  vTexCoord = aTexCoord;
}
)";

const char kFragmentShaderSource[] = R"(#version 300 es
precision mediump float;
in vec4 vColor;
in vec2 vTexCoord;
out vec4 fragColor;
void main() {
  fragColor = vColor;
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(kCubeVertices), kCubeVertices,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, kStride,
                          (GLvoid*)kPositionOffset);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, kStride,
                          (GLvoid*)kColorOffset);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, kStride,
                          (GLvoid*)kTexCoordOffset);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kCubeIndices), kCubeIndices,
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Create shader program
    program_ =
        GLCore::CreateProgram(kVertexShaderSource, kFragmentShaderSource);
  });
}

}  // namespace

TextureRenderer::TextureRenderer() : delta_(std::abs(std::rand()) % 10 + 5) {
  SetupGL();
  // glGenFramebuffers(1, &fbo_);
}

TextureRenderer::~TextureRenderer() {
  // glDeleteRenderbuffers(1, &rbo_);
  // glDeleteFramebuffers(1, &fbo_);
}

void TextureRenderer::RenderTexture(GLenum target,
                                    uint32_t texture,
                                    int32_t width,
                                    int32_t height,
                                    uint64_t timestamp) {
  CHECK_GL_ERROR();

  GLuint fbo = 0;
  GLuint rbo = 0;
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, rbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, texture,
                         0);
  // Check if the framebuffer is complete
  GLenum retval = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  FATAL_IF(retval != GL_FRAMEBUFFER_COMPLETE,
           "Framebuffer is not complete: 0x%{public}X", retval);

  RenderFrame(width, height, timestamp);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, 0, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glDeleteBuffers(1, &fbo);
  glDeleteRenderbuffers(1, &rbo);

  CHECK_GL_ERROR();
}

void TextureRenderer::RenderFrame(int32_t width,
                                  int32_t height,
                                  uint64_t timestamp) {
  // LOGE(
  //     "RenderFrame called with width: %{public}d, height: %{public}d, "
  //     "timestamp: %{public}llu",
  //     width, height, timestamp);
  auto time = std::chrono::duration_cast<std::chrono::milliseconds>(
                  std::chrono::system_clock::now() - kStartTime)
                  .count();

  auto compute_color = [time, this](int period) {
    period *= delta_;
    float result = time % (period * 2) - period;
    return std::abs(result / period);
  };

  // Set the viewport
  glViewport(0, 0, width, height);

  glClearColor(compute_color(1000), compute_color(3000), compute_color(2000),
               1.0f);
  glClearDepthf(1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Use the shader program
  glUseProgram(program_);

  auto compute_angle = [time, this](int period) {
    period *= delta_;
    return 2 * M_PI * (time % period) / period;
  };

  float angle_x = compute_angle(3000);
  float angle_y = compute_angle(2000);
  float angle_z = compute_angle(1000);

  // Rotate the triangle by angle{X,Y,Z}
  Matrix4x4 rotation_matrix = Matrix4x4::Rotate(angle_x, angle_y, angle_z) *
                              Matrix4x4::Scale(0.5f, 0.5f, 0.5f);

  GLint rotation_matrix_location =
      glGetUniformLocation(program_, "uRotationMatrix");
  glUniformMatrix4fv(rotation_matrix_location, 1, GL_FALSE,
                     rotation_matrix.data);

  // set cull
  glEnable(GL_CULL_FACE);

  // Draw the cube
  glBindVertexArray(vao_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

}  // namespace hello
