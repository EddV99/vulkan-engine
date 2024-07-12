/**
 * @file scene.hpp
 *
 * @brief header file for scene class
 *
 * @details A scene holds information for each
 *          object to be rendered and other
 *          information like camera stuff
 *
 */

#pragma once

#include "../game/camera.hpp"
#include "../math/vector.hpp"
#include "object.hpp"

#include <array>
#include <vector>

namespace Game {

struct ModelInfo {
  std::string meshFilePath;
  std::string textureFilePath;
  Math::Vector3 position;
  Math::Vector3 rotation;
  Math::Vector3 scale;
  Object::RenderMode renderMode;
};

class Scene {
private:
  std::vector<ModelInfo> models;
  size_t textureCount = 0;
  bool hasEnvironmentMap = false;
  std::array<std::string, 6> envMapImagePaths;

public:
  Scene() = default;
  ~Scene();
  Scene(const std::vector<ModelInfo> &models, std::array<std::string, 6> environmentMapImagePaths = {});
  Scene(const Scene &other);
  Scene(Scene &&other) noexcept;
  Scene &operator=(const Scene &other);
  Scene &operator=(Scene &&other) noexcept;
  const Object &operator[](int32_t index);

  void init();
  size_t getTextureCount();

  std::vector<Object> objects;
  Camera camera;
};
} // namespace Game
