/**
 * @file scene.hpp
 *
 * @brief header file for scene class
 *
 */

#pragma once

#include "../game/camera.hpp"
#include "../math/matrix.hpp"
#include "../math/vector.hpp"
#include "object.hpp"

#include <vector>

namespace Game {
class Scene {
private:
  struct ModelInfo {
    std::string meshFilePath;
    std::string textureFilePath;
    Math::Vector3 position;
    Math::Vector3 rotation;
    Math::Vector3 scale;
  };

  std::vector<ModelInfo> models;

public:
  Scene() = default;
  ~Scene();
  Scene(const std::vector<ModelInfo> &models);
  Scene(const Scene &other);
  Scene(Scene &&other) noexcept;
  Scene &operator=(const Scene &other);
  Scene &operator=(Scene &&other) noexcept;
  const Mesh::Mesh &operator[](int32_t index);

  void init();

  std::vector<Object> objects;
  Camera camera;

  Math::Matrix4 viewMatrix(Math::Vector3 up);
};
} // namespace Game
