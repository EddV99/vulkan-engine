/**
 * @file scene.hpp
 *
 * @brief header file for scene class
 *
 */

#pragma once

#include "object.hpp"
#include <vector>

namespace Game {
class Scene {
private:
public:
  Scene() = default;
  ~Scene();
  Scene(const std::vector<Object> &objects);
  Scene(const Scene &other);
  Scene(Scene &&other) noexcept;
  Scene &operator=(const Scene &other);
  Scene &operator=(Scene &&other) noexcept;

  std::vector<Object> gameObjects;
};
} // namespace Game
