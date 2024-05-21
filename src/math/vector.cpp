#include "vector.hpp"

#include <cmath>

namespace Math {
// =============================================================================
// Vector 2
// =============================================================================
Vector2::Vector2() {
  this->x = 0;
  this->y = 0;
}

Vector2::Vector2(f32 x, f32 y) {
  this->x = x;
  this->y = y;
}

// =============================================================================
// Vector 3
// =============================================================================
Vector3::Vector3() : x(0), y(0), z(0) {}
Vector3::Vector3(f32 x, f32 y, f32 z) : x(x), y(y), z(z) {}

bool Vector3::operator==(const Vector3 &right) const {
  return (this->x == right.x) && (this->y == right.y) && (this->z == right.z);
}
Vector3 Vector3::operator+(Vector3 &right) {
  return Vector3(x + right.x, this->y + right.y, this->z + right.z);
}

Vector3 Vector3::operator-(Vector3 &right) {
  return Vector3(x - right.x, this->y - right.y, this->z - right.z);
}

Vector3 &Vector3::operator=(Vector3 other) noexcept {
  x = other.x;
  y = other.y;
  z = other.z;
  return *this;
}

f32 Vector3::dot(Vector3 &right) {
  return this->x * right.x + this->y * right.y + this->z * right.z;
}

Vector3 Vector3::cross(Vector3 &right) {
  return Vector3(this->y * right.z - this->z * right.y,
                 -(this->x * right.z - this->z * right.x),
                 this->x * right.y - this->y * right.x);
}

f32 Vector3::length() {
  return std::sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
}

void Vector3::normalize() {
  f32 l = 1.0f / this->length();
  x = x * l;
  y = y * l;
  z = z * l;
}
Vector3 Vector3::normal() {
  f32 l = 1.0f / this->length();
  Vector3 result;
  result.x = x * l;
  result.y = y * l;
  result.z = z * l;
  return result;
}

// =============================================================================
// Vector 4
// =============================================================================
Vector4::Vector4() {
  this->x = 0;
  this->y = 0;
  this->z = 0;
  this->w = 0;
}

Vector4::Vector4(f32 x, f32 y, f32 z, f32 w) {
  this->x = x;
  this->y = y;
  this->z = z;
  this->w = w;
}

}; // namespace Math
