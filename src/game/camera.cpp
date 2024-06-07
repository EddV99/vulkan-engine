#include "camera.hpp"

namespace Game {
Camera::Camera() : position{-20, 0, 0}, target{0, 0, 0}, up(0, 1, 0) {}

Camera::Camera(const Camera &other) : position(other.position), target(other.target), up(other.up) {}

Camera::Camera(Camera &&other) noexcept : position(other.position), target(other.target), up(other.up) {}

Camera &Camera::operator=(const Camera &other) {
  if (this == &other)
    return *this;

  this->position = other.position;
  this->target = other.target;
  this->up = other.up;

  return *this;
}
Camera &Camera::operator=(Camera &&other) noexcept {
  if (this == &other)
    return *this;

  this->position = other.position;
  this->target = other.target;
  this->up = other.up;

  return *this;
}
void Camera::movePosition(Math::Vector3 dp) { position = position + dp; }
void Camera::movePositionX(f32 dx) { position.x += dx; }
void Camera::movePositionY(f32 dy) { position.y += dy; }
void Camera::movePositionZ(f32 dz) { position.z += dz; }
}; // namespace Game
