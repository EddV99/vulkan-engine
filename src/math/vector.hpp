/**
 * @brief
 */

#pragma once

#include "../util/defines.hpp"

namespace Math {

class Vector3 {
public:
  Vector3(f32 x = 0, f32 y = 0, f32 z = 0);

  Vector3 operator+(Vector3 &right);
  Vector3 operator-(Vector3 &right);
  Vector3 &operator=(Vector3 other) noexcept;

  f32 dot(Vector3 &right);
  Vector3 cross(Vector3 &right);
  f32 length();
  void normalize();
  Vector3 normal();

private:
  f32 x, y, z;
};

class Vector4 {
public:
  Vector4(f32 x = 0, f32 y = 0, f32 z = 0, f32 w = 0);

  Vector4 operator+(Vector4 &right);
  Vector4 operator-(Vector4 &right);
  Vector4 &operator=(Vector4 other) noexcept;

  f32 dot(Vector4 &right);
  Vector4 cross(Vector4 &right);
  f32 length();
  void normalize();
  Vector4 normal();

private:
  f32 x, y, z, w;
};

}; // namespace Math
