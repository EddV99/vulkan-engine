/**
 *
 */
#pragma once

#include "../math/vector.hpp"

namespace Scene {
class Camera {
public:
private:
  Math::Vector3 position;
  Math::Vector3 target;
  Math::Vector3 up;
};
}; // namespace Scene
