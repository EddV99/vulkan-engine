#include "vector.hpp"
#include <cmath>
namespace Math {

// =============================================================================
// Vector 3
// =============================================================================

Vector3::Vector3(float x, float y, float z) {
  data[0] = x;
  data[1] = y;
  data[2] = z;
}

float Vector3::x() { return data[0]; }
float Vector3::y() { return data[1]; }
float Vector3::z() { return data[2]; }

Vector3 Vector3::operator+(Vector3 &right) {
  return Vector3(this->data[0] + right.data[0], this->data[1] + right.data[1],
                 this->data[2] + right.data[2]);
}

Vector3 Vector3::operator-(Vector3 &right) {
  return Vector3(this->data[0] - right.data[0], this->data[1] - right.data[1],
                 this->data[2] - right.data[2]);
}

Vector3 &Vector3::operator=(Vector3 other) noexcept {
  this->data[0] = other.data[0];
  this->data[1] = other.data[1];
  this->data[2] = other.data[2];
  return *this;
}

float Vector3::dot(Vector3 &right) {
  return this->x() * right.x() + this->y() * right.y() + this->z() * right.z();
}

Vector3 Vector3::cross(Vector3 &right) {
  return Vector3(this->y() * right.z() - this->z() * right.y(),
                 -(this->x() * right.z() - this->z() * right.x()),
                 this->x() * right.y() - this->y() * right.x());
}

float Vector3::length() {
  return std::sqrt(this->x() * this->x() + this->y() * this->y() +
                   this->z() * this->z());
}

void Vector3::normalize() {
  float l = 1.0f / this->length();
  data[0] = data[0] * l;
  data[1] = data[1] * l;
  data[2] = data[2] * l;
}
Vector3 Vector3::normal() {
  float l = 1.0f / this->length();
  Vector3 result;
  result.data[0] = data[0] * l;
  result.data[1] = data[1] * l;
  result.data[2] = data[2] * l;
  return result;
}

// =============================================================================
// Vector 4
// =============================================================================

Vector4::Vector4(float x, float y, float z, float w) {
  data[0] = x;
  data[1] = y;
  data[2] = z;
  data[3] = w;
}

float Vector4::x() { return data[0]; }
float Vector4::y() { return data[1]; }
float Vector4::z() { return data[2]; }
float Vector4::w() { return data[3]; }

}; // namespace Math
