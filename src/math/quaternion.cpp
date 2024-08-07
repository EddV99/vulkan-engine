#include "quaternion.hpp"
#include "../util/defines.hpp"
#include <cmath>
#include <iostream>

namespace Math {
// =============================================================================
// Quaternions
// =============================================================================
Quaternion::Quaternion(f32 w, f32 i, f32 j, f32 k) : w(w), v{i, j, k} {}

Quaternion::Quaternion(f32 angle, const Vector3 &axis) {
  angle = TO_RADIANS(angle);
  f32 sinHalf = sinf(angle * 0.5);
  f32 cosHalf = cosf(angle * 0.5);

  v.x = axis.x * sinHalf;
  v.y = axis.y * sinHalf;
  v.z = axis.z * sinHalf;
  w = cosHalf;
}
Quaternion::Quaternion(const Quaternion &other) : w(other.w), v(other.v) {}

Quaternion::Quaternion(Quaternion &&other) noexcept : w(other.w), v(other.v) {}

void Quaternion::print() { std::cout << "(" << w << "{" << v.x << " " << v.y << " " << v.z << "})\n"; }

Quaternion &Quaternion::operator=(Quaternion &&other) noexcept {
  if (this == &other)
    return *this;

  this->v = other.v;
  this->w = other.w;

  return *this;
}

Quaternion &Quaternion::operator=(const Quaternion &other) {
  if (this == &other)
    return *this;

  this->v = other.v;
  this->w = other.w;

  return *this;
}

Quaternion Quaternion::operator*(const Quaternion &other) {
  f32 x0 = w * other.w - v.dot(other.v);
  Math::Vector3 x = (other.v * w) + (v * other.w) + v.cross(other.v);
  return Quaternion{x0, x.x, x.y, x.z};
}
Quaternion Quaternion::operator*(f32 scalar) {
  return Quaternion(w * scalar, v.x * scalar, v.y * scalar, v.z * scalar);
}
Quaternion Quaternion::operator+(const Quaternion &other) {
  return Quaternion(w + other.w, v.x + other.v.x, v.y + other.v.y, v.z + other.v.z);
}

void Quaternion::rotate(Vector3 r) {
  f32 x0 = std::cos(TO_RADIANS(r.x) / 2.0f);
  f32 x1 = std::sin(TO_RADIANS(r.x) / 2.0f);

  f32 y0 = std::cos(TO_RADIANS(r.y) / 2.0f);
  f32 y1 = std::sin(TO_RADIANS(r.y) / 2.0f);

  f32 z0 = std::cos(TO_RADIANS(r.z) / 2.0f);
  f32 z1 = std::sin(TO_RADIANS(r.z) / 2.0f);

  f32 q0 = x0 * y0 * z0 + x1 * y1 * z1;
  f32 q1 = x1 * y0 * z0 - x0 * y1 * z1;
  f32 q2 = x0 * y1 * z0 + x1 * y0 * z1;
  f32 q3 = x0 * y0 * z1 - x1 * y1 * z0;

  this->w = q0;
  this->v = {q1, q2, q3};
}

f32 Quaternion::length() { return std::sqrt(w * w + v.x * v.x + v.y * v.y + v.z * v.z); }

Quaternion Quaternion::conjugate() { return Quaternion{w, Vector3(v * -1.0f)}; }

void Quaternion::normalize() {
  f32 length = 1.0f / this->length();
  this->w *= length;
  this->v.x *= length;
  this->v.y *= length;
  this->v.z *= length;
}

Matrix4 Quaternion::toRotationMatrix() {
  this->normalize();
  /* return Matrix4(2 * (w * w + v.x * v.x) - 1, 2 * (v.x * v.y - w * v.z), 2 * (v.x * v.z + w * v.y), 0, // */
  /*                2 * (v.x * v.y + w * v.z), 2 * (w * w + v.y * v.y) - 1, 2 * (v.y * v.z - w * v.x), 0, // */
  /*                2 * (v.x * v.z - w * v.y), 2 * (v.y * v.z + w * v.x), 2 * (w * w + v.z * v.z) - 1, 0, // */
  /*                0, 0, 0, 1); */

  f32 ww = w * w;
  f32 xx = v.x * v.x;
  f32 yy = v.y * v.y;
  f32 zz = v.z * v.z;

  f32 x = v.x;
  f32 y = v.y;
  f32 z = v.z;

  return Matrix4(ww + xx - yy - zz, 2 * x * y - 2 * w * z, 2 * x * z + 2 * w * y, 0, //
                 2 * x * y + 2 * w * z, ww - xx + yy - zz, 2 * y * z - 2 * w * x, 0, //
                 2 * x * z - 2 * w * y, 2 * y * z + 2 * w * x, ww - xx - yy + zz, 0, //
                 0, 0, 0, 1);
}
Quaternion Quaternion::rotateX(f32 angle) {
  f32 a = angle / 2;
  return Quaternion(std::cos(a), std::sin(a), 0, 0);
};
Quaternion Quaternion::rotateY(f32 angle) {
  f32 a = angle / 2;
  return Quaternion(std::cos(a), 0, std::sin(a), 0);
};
Quaternion Quaternion::rotateZ(f32 angle) {
  f32 a = angle / 2;
  return Quaternion(std::cos(a), 0, 0, std::sin(a));
};
}; // namespace Math
