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

Math::Matrix4 Scene::viewMatrix(Math::Vector3 up) {
  Math::Vector3 forward = camera.target - camera.position;
  forward.normalize();

  Math::Vector3 right = up.cross(forward);
  right.normalize();

  Math::Vector3 cameraUp = forward.cross(right);
  cameraUp.normalize();

  return Math::Matrix4(right.x, cameraUp.x, forward.x, 0, //
                       right.y, cameraUp.y, forward.y, 0, //
                       right.z, cameraUp.z, forward.z, 0, //
                       -camera.position.x, -camera.position.y, -camera.position.z, 1);
}

} // namespace Game
