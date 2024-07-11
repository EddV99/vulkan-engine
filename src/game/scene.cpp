/**
 * @file scene.cpp
 */

#include "scene.hpp"

#include <algorithm>
#include <vector>

namespace Game {

Scene::~Scene() { objects.clear(); }

Scene::Scene(const std::vector<ModelInfo> &models) : models(models) { camera = Camera(); }

Scene::Scene(const Scene &other)
    : models(other.models), textureCount(other.textureCount), objects(other.objects), camera(other.camera) {}

Scene::Scene(Scene &&other) noexcept
    : models(other.models), textureCount(other.textureCount), objects(other.objects), camera(other.camera) {
  other.models.clear();
  other.objects.clear();
}

Scene &Scene::operator=(const Scene &other) {
  if (this == &other)
    return *this;

  this->models = other.models;
  this->textureCount = other.textureCount;
  this->objects = other.objects;
  this->camera = other.camera;

  return *this;
}

Scene &Scene::operator=(Scene &&other) noexcept {
  if (this == &other)
    return *this;

  this->models = other.models;
  this->textureCount = other.textureCount;
  this->objects = other.objects;
  this->camera = other.camera;

  other.models.clear();
  other.objects.clear();

  return *this;
}

const Object &Scene::operator[](int32_t index) { return objects[index]; }

size_t Scene::getTextureCount() { return textureCount; }

void Scene::init() {
  for (const auto &modelInfo : models) {
    Math::Quaternion q = {0, {0, 0, 0}};
    q.rotate(modelInfo.rotation);
    Object obj(modelInfo.position, q, modelInfo.scale, modelInfo.renderMode);
    obj.init(modelInfo.meshFilePath, modelInfo.textureFilePath);
    objects.push_back(obj);

    if (!modelInfo.textureFilePath.empty())
      textureCount++;
  }

  // sort by render mode
  std::sort(objects.begin(), objects.end());
}

} // namespace Game
