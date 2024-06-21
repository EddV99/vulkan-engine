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

  double ax = 0, ay = 0;

  void movePosition(Math::Vector3 dp);
  void movePositionX(f32 dx);
  void movePositionY(f32 dy);
  void movePositionZ(f32 dz);

  Math::Matrix4 viewMatrix();

  void toggleFreecam();

private:
  bool freecam = false;
};
}; // namespace Game
