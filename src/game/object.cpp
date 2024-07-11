#include "object.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../util/stb_image.h"

#include "../util/util.hpp"

namespace Game {
Object::Object(Math::Vector3 p, Math::Quaternion r, Math::Vector3 s, RenderMode m)
    : position(p), rotation(r), scale(s), renderMode(m) {}

Object::Object(const Object &other)
    : position(other.position), rotation(other.rotation), scale(other.scale), mesh(other.mesh),
      renderMode(other.renderMode) {}

Object &Object::operator=(const Object &other) {
  if (this == &other)
    return *this;
  this->mesh = other.mesh;
  this->rotation = other.rotation;
  this->position = other.position;
  this->scale = other.scale;
  this->renderMode = other.renderMode;
  return *this;
}

Object::Object(Object &&other) noexcept
    : position(other.position), rotation(other.rotation), scale(other.scale), mesh(other.mesh),
      renderMode(other.renderMode) {

  other.rotation = {0, {0, 0, 0}};
  other.position = {0, 0, 0};
  other.scale = {0, 0, 0};
}

Object &Object::operator=(Object &&other) noexcept {
  if (this == &other)
    return *this;

  this->mesh = other.mesh;
  this->rotation = other.rotation;
  this->position = other.position;
  this->scale = other.scale;
  this->renderMode = other.renderMode;

  other.rotation = {0, {0, 0, 0}};
  other.position = {0, 0, 0};
  other.scale = {0, 0, 0};

  return *this;
}

bool Object::operator<(const Object &other) { return this->renderMode < other.renderMode; }

void Object::init(std::string meshPath, std::string texturePath) {
  mesh.init(meshPath);

  if (!texturePath.empty())
    loadTexture(texturePath);
}

Math::Matrix4 Object::getModelMatrix() {
  Math::Matrix4 model(1.0, 0.0, 0.0, 0.0, //
                      0.0, 1.0, 0.0, 0.0, //
                      0.0, 0.0, 1.0, 0.0, //
                      0.0, 0.0, 0.0, 1.0);
  model = getScaleMatrix(scale) * model;
  model = getTranslationMatrix(mesh.getBoundingBox().mid * -scale.x) * model;
  /* model = getRotationMatrix() * model; */
  model = rotation.toRotationMatrix() * model;
  model = getTranslationMatrix(position) * model;
  return model;
}

Math::Matrix4 Object::getTranslationMatrix(Math::Vector3 t) {
  return Math::Matrix4(1, 0, 0, t.x, //
                       0, 1, 0, t.y, //
                       0, 0, 1, t.z, //
                       0, 0, 0, 1);
}

Math::Matrix4 Object::getRotationMatrix() {
  f32 cosx = cosf(TO_RADIANS(pitch));
  f32 sinx = sinf(TO_RADIANS(pitch));
  f32 cosy = cosf(TO_RADIANS(yaw));
  f32 siny = sinf(TO_RADIANS(yaw));
  f32 cosz = cosf(TO_RADIANS(roll));
  f32 sinz = sinf(TO_RADIANS(roll));
  return Math::Matrix4(cosy * cosz, sinx * siny * cosz - cosx * sinz, cosx * siny * cosz + sinx * sinz, 0, //
                       cosy * sinz, sinx * siny * sinz + cosx * cosz, cosx * siny * sinz - sinx * cosz, 0, //
                       -siny, sinx * cosy, cosx * cosy, 0,                                                 //
                       0, 0, 0, 1);
}

Math::Matrix4 Object::getScaleMatrix(Math::Vector3 s) {
  return Math::Matrix4(s.x, 0, 0, 0, //
                       0, s.y, 0, 0, //
                       0, 0, s.z, 0, //
                       0, 0, 0, 1);
}

void Object::loadTexture(std::string fileName) {
  int width, height, channels;

  texture.pixels = stbi_load(fileName.c_str(), &width, &height, &channels, STBI_rgb_alpha);
  if (!texture.pixels)
    Util::Error("Failed to load texture image: " + fileName);

  texture.width = width;
  texture.height = height;
  texture.channels = channels;

  textureLoaded = true;
}
bool Object::hasTexture() { return textureLoaded; }

void Object::removeTexture() {
  if (texture.pixels) {
    stbi_image_free(texture.pixels);
    texture.pixels = nullptr;
    textureLoaded = false;
  }
}

const Object::TextureData &Object::getTextureData() { return texture; }
const Mesh::Mesh &Object::getMesh() { return mesh; }

void Object::setRotation(Math::Vector3 r) {
  pitch = r.x;
  yaw = r.y;
  roll = r.z;
  rotation.rotate(r);
}

void Object::setRotationX(f32 x) {
  pitch = x;
  rotation.rotate({pitch, yaw, roll});
}

void Object::setRotationY(f32 y) {
  yaw = y;
  rotation.rotate({pitch, yaw, roll});
}

void Object::setRotationZ(f32 z) {
  roll = z;
  rotation.rotate({pitch, yaw, roll});
}

void Object::moveRotation(Math::Vector3 dr) {
  pitch += dr.x;
  yaw += dr.y;
  roll += dr.z;
  rotation.rotate({pitch, yaw, roll});
}
void Object::moveRotationX(f32 dx) {
  pitch += dx;
  rotation.rotate({pitch, yaw, roll});
}

void Object::moveRotationY(f32 dy) {
  yaw += dy;
  rotation.rotate({pitch, yaw, roll});
}

void Object::moveRotationZ(f32 dz) {
  roll += dz;
  rotation.rotate({pitch, yaw, roll});
}

void Object::setPosition(Math::Vector3 p) { position = p; }
void Object::setPositionX(f32 x) { position.x = x; }
void Object::setPositionY(f32 y) { position.y = y; }
void Object::setPositionZ(f32 z) { position.z = z; }

void Object::movePosition(Math::Vector3 dp) { position = position + dp; }
void Object::movePositionX(f32 dx) { position.x = position.x + dx; }
void Object::movePositionY(f32 dy) { position.y = position.y + dy; }
void Object::movePositionZ(f32 dz) { position.z = position.z + dz; }

} // namespace Game
