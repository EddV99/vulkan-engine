/**
 * @brief
 */

#pragma once

namespace Math {
class Vector3 {
public:
  Vector3(float x = 0, float y = 0, float z = 0);

  float x();
  float y();
  float z();

  Vector3 operator+(Vector3 &right);
  Vector3 operator-(Vector3 &right);
  Vector3 &operator=(Vector3 other) noexcept;

  float dot(Vector3 &right);
  Vector3 cross(Vector3 &right);
  float length();
  void normalize();
  Vector3 normal();

private:
  float data[3];
};

class Vector4 {
public:
  Vector4(float x = 0, float y = 0, float z = 0, float w = 0);

  float x();
  float y();
  float z();
  float w();

  Vector4 operator+(Vector4 &right);
  Vector4 operator-(Vector4 &right);
  Vector4 &operator=(Vector4 other) noexcept;

  float dot(Vector4 &right);
  Vector4 cross(Vector4 &right);
  float length();
  void normalize();
  Vector4 normal();

private:
  float data[4];
};

}; // namespace Math
