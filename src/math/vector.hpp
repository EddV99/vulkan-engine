/**
 * @brief
 */
#pragma once

#include "../util/defines.hpp"

#include <unordered_map>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace Math {

class Vector3 {
public:
  f32 x, y, z;

  Vector3();
  Vector3(f32 x, f32 y, f32 z);

  Vector3 operator+(Vector3 &right);
  Vector3 operator-(Vector3 &right);
  Vector3 &operator=(Vector3 other) noexcept;
  bool operator==(const Vector3 &right) const;

  f32 dot(Vector3 &right);
  Vector3 cross(Vector3 &right);
  f32 length();
  void normalize();
  Vector3 normal();
};

class Vector4 {
public:
  f32 x, y, z, w;

  Vector4();
  Vector4(f32 x, f32 y, f32 z, f32 w);

  Vector4 operator+(Vector4 &right);
  Vector4 operator-(Vector4 &right);
  Vector4 &operator=(Vector4 other) noexcept;

  f32 dot(Vector4 &right);
  Vector4 cross(Vector4 &right);
  f32 length();
  void normalize();
  Vector4 normal();
};

class Vector2 {
public:
  f32 x, y;

  Vector2();
  Vector2(f32 x, f32 y);

  Vector2 operator+(Vector2 &right);
  Vector2 operator-(Vector2 &right);
  Vector2 &operator=(Vector2 other) noexcept;

  f32 dot(Vector2 &right);
  Vector2 cross(Vector2 &right);
  f32 length();
  void normalize();
  Vector2 normal();
};

}; // namespace Math

namespace std {
template <> struct hash<Math::Vector3> {
  size_t operator()(Math::Vector3 const &vertex) const {
    size_t h1 = std::hash<f32>()(vertex.x);
    size_t h2 = std::hash<f32>()(vertex.y);
    size_t h3 = std::hash<f32>()(vertex.z);
    return h1 ^ (h2 << 1) ^ h3;
  }
};
} // namespace std
