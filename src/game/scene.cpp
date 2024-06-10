/**
 * @file scene.cpp
 */

#include "scene.hpp"
#include <vector>

namespace Game {

Scene::~Scene() { gameObjects.clear(); }

Scene::Scene(const std::vector<Object> &objects) : gameObjects(objects) { camera = Camera(); }

Scene::Scene(const Scene &other) : gameObjects(other.gameObjects) { camera = other.camera; }

Scene::Scene(Scene &&other) noexcept : gameObjects(other.gameObjects) { other.gameObjects.clear(); }

Scene &Scene::operator=(const Scene &other) {
  if (this == &other)
    return *this;

  this->gameObjects = other.gameObjects;
  this->camera = other.camera;

  return *this;
}

Scene &Scene::operator=(Scene &&other) noexcept {
  if (this == &other)
    return *this;

  this->gameObjects = other.gameObjects;
  other.gameObjects.clear();

  this->camera = other.camera;

  return *this;
}

Math::Matrix4 Scene::viewMatrix(Math::Vector3 up) {
  // Math::Vector3 forward = camera.target - camera.position;
  Math::Vector3 forward = camera.position - camera.target;
  forward.normalize();

  Math::Vector3 right = up.cross(forward);
  right.normalize();

  Math::Vector3 cameraUp = forward.cross(right);
  cameraUp.normalize();

  /* return Math::Matrix4(right.x, cameraUp.x, forward.x, 0, // */
  /*                      right.y, cameraUp.y, forward.y, 0, // */
  /*                      right.z, cameraUp.z, forward.z, 0, // */
  /*                      -camera.position.x, -camera.position.y, -camera.position.z, 1); */
  return Math::Matrix4(right.x, right.y, right.z, -camera.position.x,          //
                       cameraUp.x, cameraUp.y, cameraUp.z, -camera.position.y, //
                       forward.x, forward.y, forward.z, -camera.position.z,    //
                       0, 0, 0, 1);
}

} // namespace Game
