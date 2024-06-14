/**
 * @file scene.cpp
 */

#include "scene.hpp"
#include <vector>

namespace Game {

Scene::~Scene() { objects.clear(); }

Scene::Scene(const std::vector<ModelInfo> &models) : models(models) { camera = Camera(); }

Scene::Scene(const Scene &other) : objects(other.objects) { camera = other.camera; }

Scene::Scene(Scene &&other) noexcept : objects(other.objects) { other.objects.clear(); }

Scene &Scene::operator=(const Scene &other) {
  if (this == &other)
    return *this;

  this->objects = other.objects;
  this->camera = other.camera;

  return *this;
}

Scene &Scene::operator=(Scene &&other) noexcept {
  if (this == &other)
    return *this;

  this->objects = other.objects;
  other.objects.clear();

  this->camera = other.camera;

  return *this;
}

const Mesh::Mesh &Scene::operator[](int32_t index) { return objects[index].getMesh(); }

void Scene::init() {
  for (const auto &modelInfo : models) {
    Object obj(modelInfo.position, {0, {0, 0, 0}}, modelInfo.scale);
    obj.init(modelInfo.meshFilePath, modelInfo.textureFilePath);
  }
}

Math::Matrix4 Scene::viewMatrix(Math::Vector3 up) {
  // Math::Vector3 forward = camera.target - camera.position;
  Math::Vector3 forward = camera.position - camera.target;
  forward.normalize();

  Math::Vector3 right = up.cross(forward);
  right.normalize();

  Math::Vector3 cameraUp = forward.cross(right);
  cameraUp.normalize();

  return Math::Matrix4(right.x, right.y, right.z, -camera.position.x,          //
                       cameraUp.x, cameraUp.y, cameraUp.z, -camera.position.y, //
                       forward.x, forward.y, forward.z, -camera.position.z,    //
                       0, 0, 0, 1);
}
} // namespace Game
