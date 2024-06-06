/**
 *
 */
#pragma once

#include "../math/vector.hpp"

namespace Game {
class Camera {
public:
  Math::Vector3 position;
  Math::Vector3 target;
  Math::Vector3 up;
private:
};
}; // namespace Game
