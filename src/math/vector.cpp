#include "vector.hpp"

#include <cmath>
#include <iostream>

namespace Math {
// =============================================================================
// Vector 2
// =============================================================================
Vector2::Vector2() : x(0), y(0) {}
Vector2::Vector2(f32 x, f32 y) : x(x), y(y) {}

bool Vector2::operator==(const Vector2 &right) const { return (this->x == right.x) && (this->y == right.y); }

Vector2 Vector2::operator+(Vector2 &right) { return Vector2(x + right.x, this->y + right.y); }

Vector2 Vector2::operator*(f32 scalar) { return Vector2(x * scalar, y * scalar); }

Vector2 Vector2::operator-(Vector2 &right) { return Vector2(x - right.x, this->y - right.y); }

/* Vector2 &Vector2::operator=(Vector2 other) noexcept { */
/*   x = other.x; */
/*   y = other.y; */
/*   return *this; */
/* } */

// =============================================================================
// Vector 3
// =============================================================================
Vector3::Vector3() : x(0), y(0), z(0) {}
Vector3::~Vector3() {
  this->x = 0;
  this->y = 0;
  this->z = 0;
}
Vector3::Vector3(f32 x, f32 y, f32 z) : x(x), y(y), z(z) {}

Vector3::Vector3(const Vector3 &other) : x(other.x), y(other.y), z(other.z) {}

void Vector3::print() const { std::cout << "[" << x << " " << y << " " << z << "]\n"; }

Vector3 &Vector3::operator=(const Vector3 &other) {
  if (this == &other)
    return *this;

  this->x = other.x;
  this->y = other.y;
  this->z = other.z;

  return *this;
}

Vector3::Vector3(Vector3 &&other) noexcept : x(other.x), y(other.y), z(other.z) {
  other.x = 0;
  other.y = 0;
  other.z = 0;
}
Vector3 &Vector3::operator=(Vector3 &&other) noexcept {
  if (this == &other)
    return *this;

  this->x = other.x;
  this->y = other.y;
  this->z = other.z;

  other.x = 0;
  other.y = 0;
  other.z = 0;

  return *this;
}

bool Vector3::operator==(const Vector3 &right) const {
  return (this->x == right.x) && (this->y == right.y) && (this->z == right.z);
}
Vector3 Vector3::operator+(const Vector3 &right) const { return Vector3(x + right.x, y + right.y, z + right.z); }

Vector3 Vector3::operator*(f32 scalar) const { return Vector3(x * scalar, y * scalar, z * scalar); }

Vector3 Vector3::operator-(const Vector3 &right) const { return Vector3(x - right.x, y - right.y, z - right.z); }

f32 Vector3::dot(const Vector3 &right) const { return x * right.x + y * right.y + z * right.z; }

Vector3 Vector3::cross(const Vector3 &right) const {
  return Vector3(this->y * right.z - this->z * right.y, -(this->x * right.z - this->z * right.x),
                 this->x * right.y - this->y * right.x);
}

f32 Vector3::length() const { return std::sqrt(this->x * this->x + this->y * this->y + this->z * this->z); }

void Vector3::normalize() {
  f32 l = 1.0f / this->length();
  x = x * l;
  y = y * l;
  z = z * l;
}
Vector3 Vector3::normal() const {
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

Vector4 Vector4::operator*(f32 scalar) const { return Vector4(x * scalar, y * scalar, z * scalar, w * scalar); }

}; // namespace Math
