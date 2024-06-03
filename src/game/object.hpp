#include "../math/matrix.hpp"
#include "../math/vector.hpp"
#include "../mesh/mesh.hpp"

namespace Game {
class Object {
private:
  Mesh::Mesh mesh;
  Math::Quaternion orientation;
  Math::Vector3 position;
  Math::Vector3 scale;

public:
  Object(std::string meshFilename, Math::Vector3 p = {0.0, 0.0, 0.0}, Math::Quaternion o = {0.0, {0.0, 0.0, 1.0}},
         Math::Vector3 s = {1.0, 1.0, 1.0});

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

  Math::Matrix4 getModelMatrix();
};
} // namespace Game
