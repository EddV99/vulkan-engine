/**
 *
 */
#pragma once

#include "../math/vector.hpp"

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

  void movePosition(Math::Vector3 dp);
  void movePositionX(f32 dx);
  void movePositionY(f32 dy);
  void movePositionZ(f32 dz);
private:
};
}; // namespace Game
