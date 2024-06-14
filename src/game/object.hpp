#pragma once

#include "../util/stb_image.h"

#include "../math/matrix.hpp"
#include "../math/vector.hpp"
#include "../mesh/mesh.hpp"

namespace Game {
class Object {
private:
  struct TextureData {
    stbi_uc *pixels;
    int width = 1, height = 1, channels;
  };

  void loadTexture(std::string fileName);
  bool textureLoaded = false;

  Math::Vector3 position;
  Math::Quaternion orientation;
  Math::Vector3 scale;

  Math::Matrix4 getTranslationMatrix(Math::Vector3 t);
  Math::Matrix4 getScaleMatrix(Math::Vector3 s);

  Mesh::Mesh mesh;
  TextureData texture;

public:
  Object() = delete;
  Object(Math::Vector3 p = {0, 0, 0}, Math::Quaternion o = {0, {0, 0, 0}}, Math::Vector3 s = {1, 1, 1});
  ~Object() = default;
  Object(const Object &other);
  Object &operator=(const Object &other);
  Object(Object &&other) noexcept;
  Object &operator=(Object &&other) noexcept;

  void init(std::string meshPath, std::string texturePath = "");

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
  
  const TextureData& getTextureData();
  const Mesh::Mesh& getMesh();
};
} // namespace Game
