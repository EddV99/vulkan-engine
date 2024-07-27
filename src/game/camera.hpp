/**
 *
 */
#pragma once

#include "../math/matrix.hpp"
#include "../math/vector.hpp"
#include "../util/defines.hpp"

namespace Game {
class Camera {
public:
  Camera();
  ~Camera() = default;
  Camera(const Camera &other);
  Camera(Camera &&other) noexcept;
  Camera &operator=(const Camera &other);
  Camera &operator=(Camera &&other) noexcept;

  Math::Vector3 position;
  Math::Vector3 target;
  Math::Vector3 up;
  Math::Vector3 direction;

  double yaw = 0, pitch = 0;

  Math::Matrix4 view;

  Math::Vector3 right;
  Math::Vector3 cameraUp;
  Math::Vector3 forward;

  void movePosition(Math::Vector3 dp);
  void movePositionX(f32 dx);
  void movePositionY(f32 dy);
  void movePositionZ(f32 dz);

  void setTarget(Math::Vector3 targetPos);

  Math::Matrix4 getViewMatrix() const;
  Math::Vector3 getRightVector() const;
  Math::Vector3 getForwardVector() const;
  Math::Vector3 getUpVector() const;

  void update();

  void toggleFreecam();

private:
  bool freecam = false;
};
}; // namespace Game
