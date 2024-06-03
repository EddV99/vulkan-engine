#include "object.hpp"

namespace Game {
Object::Object(std::string meshFilename, Math::Vector3 p, Math::Quaternion o, Math::Vector3 s)
    : position(p), orientation(o), scale(s) {
  mesh.loadOBJFile(meshFilename);
  mesh.computeBoundingBox();
}
} // namespace Game
