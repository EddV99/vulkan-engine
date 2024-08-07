/**
 *
 */
#pragma once

#include "../util/defines.hpp"
#include "matrix.hpp"
#include "vector.hpp"

namespace Math {
// =============================================================================
// Quaternions
// =============================================================================
struct Quaternion {
  Quaternion() = delete;
  Quaternion(f32 angle, const Vector3 &axis);
  Quaternion(f32 w, f32 i, f32 j, f32 k);
  Quaternion(const Quaternion &other);
  Quaternion(Quaternion &&other) noexcept;
  void print();
  Quaternion &operator=(const Quaternion &other);
  Quaternion &operator=(Quaternion &&other) noexcept;

  Quaternion operator*(const Quaternion &other);
  Quaternion operator*(f32 scalar);
  Quaternion operator+(const Quaternion &other);
  Quaternion conjugate();
  void normalize();
  f32 length();

  Quaternion rotateEuler();

  static Quaternion rotateX(f32 angle);
  static Quaternion rotateY(f32 angle);
  static Quaternion rotateZ(f32 angle);

  void rotate(Vector3 r);

  Matrix4 toRotationMatrix();

  f32 w;
  Vector3 v;
};

} // namespace Math
