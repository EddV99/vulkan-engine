#include "camera.hpp"
#include <cmath>

namespace Game {
Camera::Camera() : position{0, 0, 10}, target{0, 0, 0}, up{0, 1, 0}, direction{0, 0, 1} { update(); }

Camera::Camera(const Camera &other)
    : position(other.position), target(other.target), up(other.up), direction(other.direction), yaw(other.yaw),
      pitch(other.pitch), view(other.view), right(other.right), cameraUp(other.cameraUp), forward(other.forward),
      freecam(other.freecam) {}

Camera::Camera(Camera &&other) noexcept
    : position(other.position), target(other.target), up(other.up), direction(other.direction), yaw(other.yaw),
      pitch(other.pitch), view(other.view), right(other.right), cameraUp(other.cameraUp), forward(other.forward),
      freecam(other.freecam) {}

Camera &Camera::operator=(const Camera &other) {
  if (this == &other)
    return *this;

  this->position = other.position;
  this->target = other.target;
  this->up = other.up;
  this->direction = other.direction;
  this->yaw = other.yaw;
  this->pitch = other.pitch;
  this->view = other.view;
  this->right = other.right;
  this->cameraUp = other.cameraUp;
  this->forward = other.forward;
  this->freecam = other.freecam;

  return *this;
}

Camera &Camera::operator=(Camera &&other) noexcept {
  if (this == &other)
    return *this;

  this->position = other.position;
  this->target = other.target;
  this->up = other.up;
  this->direction = other.direction;
  this->yaw = other.yaw;
  this->pitch = other.pitch;
  this->view = other.view;
  this->right = other.right;
  this->cameraUp = other.cameraUp;
  this->forward = other.forward;
  this->freecam = other.freecam;

  return *this;
}

void Camera::movePosition(Math::Vector3 dp) { position = position + dp; }
void Camera::movePositionX(f32 dx) { position.x += dx; }
void Camera::movePositionY(f32 dy) { position.y += dy; }
void Camera::movePositionZ(f32 dz) { position.z += dz; }
void Camera::setTarget(Math::Vector3 targetPos) { this->target = targetPos; }

Math::Matrix4 Camera::getViewMatrix() const { return view; }
Math::Vector3 Camera::getRightVector() const { return right; }
Math::Vector3 Camera::getForwardVector() const { return forward; }
Math::Vector3 Camera::getUpVector() const { return cameraUp; }

void Camera::update() {
  if (freecam) {
    // free fly camera
    forward = direction;
    forward.normalize();
    /* forward = position - (position + direction); */
    /* forward.normalize(); */

    right = up.cross(forward);
    right.normalize();

    cameraUp = forward.cross(right);
    cameraUp.normalize();
  } else {
    // Look at target
    forward = position - target;
    forward.normalize();

    right = up.cross(forward);
    right.normalize();

    cameraUp = forward.cross(right);
    cameraUp.normalize();
  }

  Math::Matrix4 view(right.x, right.y, right.z, 0.0f,          //
                     cameraUp.x, cameraUp.y, cameraUp.z, 0.0f, //
                     forward.x, forward.y, forward.z, 0.0f,    //
                     0, 0, 0, 1);

  Math::Matrix4 transform(1.0f, 0, 0, -position.x, //
                          0, 1.0f, 0, -position.y, //
                          0, 0, 1.0f, -position.z, //
                          0, 0, 0, 1.0f);

  this->view = view * transform;
}

void Camera::toggleFreecam() { freecam = !freecam; }
}; // namespace Game
