/**
 * @file scene.cpp
 */

#include "scene.hpp"
#include <vector>

namespace Game {

Scene::~Scene() { gameObjects.clear(); }

Scene::Scene(const std::vector<Object> &objects) : gameObjects(objects) {}

Scene::Scene(const Scene &other) : gameObjects(other.gameObjects) {}

Scene::Scene(Scene &&other) noexcept : gameObjects(other.gameObjects) { other.gameObjects.clear(); }

Scene &Scene::operator=(const Scene &other) {
  if (this == &other)
    return *this;

  this->gameObjects = other.gameObjects;

  return *this;
}

Scene &Scene::operator=(Scene &&other) noexcept {
  if (this == &other)
    return *this;

  this->gameObjects = other.gameObjects;
  other.gameObjects.clear();

  return *this;
}

} // namespace Game
