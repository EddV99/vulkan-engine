/**
 * @file scene.cpp
 */

#include "scene.hpp"
#include <vector>

namespace Game {

Scene::~Scene() { objects.clear(); }

Scene::Scene(const std::vector<ModelInfo> &models) : models(models) { camera = Camera(); }

Scene::Scene(const Scene &other) : models(other.models), objects(other.objects), camera(other.camera) {}

Scene::Scene(Scene &&other) noexcept : models(other.models), objects(other.objects), camera(other.camera) {
  other.models.clear();
  other.objects.clear();
}

Scene &Scene::operator=(const Scene &other) {
  if (this == &other)
    return *this;

  this->models = other.models;
  this->objects = other.objects;
  this->camera = other.camera;

  return *this;
}

Scene &Scene::operator=(Scene &&other) noexcept {
  if (this == &other)
    return *this;

  this->models = other.models;
  this->objects = other.objects;
  this->camera = other.camera;

  other.models.clear();
  other.objects.clear();

  return *this;
}

const Object &Scene::operator[](int32_t index) { return objects[index]; }

void Scene::init() {
  for (const auto &modelInfo : models) {
    Math::Quaternion q = {0, {0, 0, 0}};
    q.rotate(modelInfo.rotation);
    Object obj(modelInfo.position, q, modelInfo.scale);
    obj.init(modelInfo.meshFilePath, modelInfo.textureFilePath);
    objects.push_back(obj);
  }
}

Math::Matrix4 Scene::viewMatrix(Math::Vector3 up) {
  Math::Vector3 forward = camera.position - camera.target;
  forward.normalize();

  Math::Vector3 right = up.cross(forward);
  right.normalize();

  Math::Vector3 cameraUp = forward.cross(right);
  cameraUp.normalize();

  Math::Matrix4 view(right.x, right.y, right.z, 0.0f,          //
                     cameraUp.x, cameraUp.y, cameraUp.z, 0.0f, //
                     forward.x, forward.y, forward.z, 0.0f,    //
                     0, 0, 0, 1);

  Math::Matrix4 transform(1.0f, 0, 0, -camera.position.x, //
                          0, 1.0f, 0, -camera.position.y, //
                          0, 0, 1.0f, -camera.position.z, //
                          0, 0, 0, 1.0f);

  return view * transform;
}
} // namespace Game
