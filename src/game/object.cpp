#include "object.hpp"

namespace Game {
Object::Object(std::string meshFilename, Math::Vector3 p, Math::Quaternion o, Math::Vector3 s)
    : position(p), orientation(o), scale(s) {
  mesh.loadOBJFile(meshFilename);
  mesh.computeBoundingBox();
}
Object::Object(const Object &other)
    : position(other.position), orientation(other.orientation), scale(other.scale), mesh(other.mesh) {}

Object &Object::operator=(const Object &other) {
  if (this == &other)
    return *this;
  this->mesh = other.mesh;
  this->orientation = other.orientation;
  this->position = other.position;
  this->scale = other.scale;
  return *this;
}

Object::Object(Object &&other) noexcept
    : position(other.position), orientation(other.orientation), scale(other.scale), mesh(other.mesh) {

  other.mesh.data.clear();
  other.mesh.size = 0;
  other.orientation = {0, {0, 0, 0}};
  other.position = {0, 0, 0};
  other.scale = {0, 0, 0};
}
Object &Object::operator=(Object &&other) noexcept {
  if (this == &other)
    return *this;

  this->mesh = other.mesh;
  this->orientation = other.orientation;
  this->position = other.position;
  this->scale = other.scale;

  other.mesh.data.clear();
  other.mesh.size = 0;
  other.orientation = {0, {0, 0, 0}};
  other.position = {0, 0, 0};
  other.scale = {0, 0, 0};

  return *this;
}

Math::Matrix4 Object::getModelMatrix() {
  return Math::Matrix4(1.0, 0.0, 0.0, 0.0, //
                       0.0, 1.0, 0.0, 0.0, //
                       0.0, 0.0, 1.0, 0.0, //
                       0.0, 0.0, 0.0, 1.0);
}
} // namespace Game
