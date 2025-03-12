//
// Created by huang on 2025-01-21.
//

#ifndef HELLOXCOMPONENT_MATRIX_H
#define HELLOXCOMPONENT_MATRIX_H

#include <cmath>

namespace hello {

struct Matrix4x4 {
  float data[16];

  static Matrix4x4 Identity() {
    // clang-format off
    Matrix4x4 matrix = {.data = {1.0f, 0.0f, 0.0f, 0.0f,
                                 0.0f, 1.0f, 0.0f, 0.0f,
                                 0.0f, 0.0f, 1.0f, 0.0f,
                                 0.0f, 0.0f, 0.0f, 1.0f}

    };
    // clang-format on
    return matrix;
  }

  static Matrix4x4 RotateX(float angle) {
    float cosAngle = std::cos(angle);
    float sinAngle = std::sin(angle);

    // clang-format off
    Matrix4x4 matrix = {.data = {1.0f, 0.0f, 0.0f, 0.0f,
                                 0.0f, cosAngle, -sinAngle, 0.0f,
                                 0.0f, sinAngle, cosAngle, 0.0f,
                                 0.0f, 0.0f, 0.0f, 1.0f}};
    // clang-format on
    return matrix;
  }

  static Matrix4x4 RotateY(float angle) {
    float cosAngle = std::cos(angle);
    float sinAngle = std::sin(angle);

    // clang-format off
    Matrix4x4 matrix = {.data = {cosAngle, 0.0f, sinAngle, 0.0f,
                                 0.0f, 1.0f, 0.0f, 0.0f,
                                 -sinAngle, 0.0f, cosAngle, 0.0f,
                                 0.0f, 0.0f, 0.0f, 1.0f}};
    // clang-format on
    return matrix;
  }

  static Matrix4x4 RotateZ(float angle) {
    float cosAngle = std::cos(angle);
    float sinAngle = std::sin(angle);

    // clang-format off
    Matrix4x4 matrix = {.data = {cosAngle, -sinAngle, 0.0f, 0.0f,
                                 sinAngle, cosAngle, 0.0f, 0.0f,
                                 0.0f, 0.0f, 1.0f, 0.0f,
                                 0.0f, 0.0f, 0.0f, 1.0f}};
    // clang-format on
    return matrix;
  }

  static Matrix4x4 Rotate(float angleX, float angleY, float angleZ) {
    Matrix4x4 result = Identity();
    result *= RotateX(angleX);
    result *= RotateY(angleY);
    result *= RotateZ(angleZ);
    return result;
  }

  static Matrix4x4 Translate(float x, float y, float z) {
    // clang-format off
    Matrix4x4 matrix = {.data = {1.0f, 0.0f, 0.0f, x,
                                 0.0f, 1.0f, 0.0f, y,
                                 0.0f, 0.0f, 1.0f, z,
                                 0.0f, 0.0f, 0.0f, 1.0f}};
    // clang-format on
    return matrix;
  }

  static Matrix4x4 Scale(float x, float y, float z) {
    // clang-format off
    Matrix4x4 matrix = {.data = {x, 0.0f, 0.0f, 0.0f,
                                 0.0f, y, 0.0f, 0.0f,
                                 0.0f, 0.0f, z, 0.0f,
                                 0.0f, 0.0f, 0.0f, 1.0f}};
    // clang-format on
    return matrix;
  }

  Matrix4x4 operator*(const Matrix4x4& other) const {
    Matrix4x4 result = {};
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        result.data[i * 4 + j] = 0;
        for (int k = 0; k < 4; k++) {
          result.data[i * 4 + j] += data[i * 4 + k] * other.data[k * 4 + j];
        }
      }
    }
    return result;
  }

  Matrix4x4& operator*=(const Matrix4x4& other) {
    *this = *this * other;
    return *this;
  }
};

}  // namespace hello

#endif  // HELLOXCOMPONENT_MATRIX_H
