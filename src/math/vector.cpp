#include "vector.hpp"
#include <cmath>
namespace Math {

// =============================================================================
// Vector 3
// =============================================================================

Vector3::Vector3(float x, float y, float z) {
  this->x = x;
  this->y = y;
  this->z = z;
}

Vector3 Vector3::operator+(Vector3 &right) {
  return Vector3(x + right.x, this->y + right.y, this->z + right.z);
}

Vector3 Vector3::operator-(Vector3 &right) {
  return Vector3(x - right.x, this->y - right.y, this->z - right.z);
}

Vector3 &Vector3::operator=(Vector3 other) noexcept {
  x = other.x;
  this->y = other.y;
  this->z = other.z;
  return *this;
}

float Vector3::dot(Vector3 &right) {
  return this->x * right.x + this->y * right.y + this->z * right.z;
}

Vector3 Vector3::cross(Vector3 &right) {
  return Vector3(this->y * right.z - this->z * right.y,
                 -(this->x * right.z - this->z * right.x),
                 this->x * right.y - this->y * right.x);
}

float Vector3::length() {
  return std::sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
}

void Vector3::normalize() {
  float l = 1.0f / this->length();
  x = x * l;
  y = y * l;
  z = z * l;
}
Vector3 Vector3::normal() {
  float l = 1.0f / this->length();
  Vector3 result;
  result.x = x * l;
  result.y = y * l;
  result.z = z * l;
  return result;
}

// =============================================================================
// Vector 4
// =============================================================================

Vector4::Vector4(float x, float y, float z, float w) {
  this->x = x;
  this->y = y;
  this->z = z;
  this->w = w;
}

}; // namespace Math
