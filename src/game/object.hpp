#pragma once

#include "../util/stb_image.h"

#include "../math/matrix.hpp"
#include "../math/vector.hpp"
#include "../mesh/mesh.hpp"
#include "../util/defines.hpp"

namespace Game {
class Object {
public:
  enum RenderMode { BLINN_SHADING, ENVIRONMENT_MAP };

private:
  struct TextureData {
    stbi_uc *pixels = nullptr;
    int width = 1, height = 1, channels = 1;
  };

public:
  Object() = delete;
  Object(Math::Vector3 p, Math::Quaternion o, Math::Vector3 s, RenderMode mode);
  ~Object() = default;
  Object(const Object &other);
  Object &operator=(const Object &other);
  Object(Object &&other) noexcept;
  Object &operator=(Object &&other) noexcept;

  void init(const std::string &meshPath, const std::string &texturePath = {});

  void setPosition(Math::Vector3 p);
  void setPositionX(f32 x);
  void setPositionY(f32 y);
  void setPositionZ(f32 z);

  void movePosition(Math::Vector3 dp);
  void movePositionX(f32 dx);
  void movePositionY(f32 dy);
  void movePositionZ(f32 dz);

  void setRotation(Math::Vector3 r);
  void setRotationX(f32 x);
  void setRotationY(f32 y);
  void setRotationZ(f32 z);

  void moveRotation(Math::Vector3 dr);
  void moveRotationX(f32 dx);
  void moveRotationY(f32 dy);
  void moveRotationZ(f32 dz);

  void scaleUniform(f32 s);

  void removeTexture();
  bool hasTexture();

  Math::Matrix4 getModelMatrix();

  const TextureData &getTextureData();
  const Mesh::Mesh &getMesh();

  bool operator<(const Object &other);

private:
  void loadTexture(std::string fileName);
  bool textureLoaded = false;

  Math::Vector3 position;
  Math::Quaternion rotation;
  Math::Vector3 scale;

  Math::Matrix4 getTranslationMatrix(Math::Vector3 t);
  Math::Matrix4 getRotationMatrix();
  Math::Matrix4 getScaleMatrix(Math::Vector3 s);

  Mesh::Mesh mesh;
  TextureData texture;

  f32 pitch = 0, yaw = 0, roll = 0;

  RenderMode renderMode;
};
} // namespace Game
