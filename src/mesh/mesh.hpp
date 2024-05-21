/**
 * @file mesh.hpp
 *
 * @brief header file for mesh class
 *
 */
#pragma once

#include "../math/vector.hpp"

#include <string>
#include <vector>

namespace Mesh {
class Mesh {
public:
  Mesh() = default;

  void computeBoundingBox();

  bool hasNormals();
  bool hasUV();

  void loadOBJFile(std::string filename);

  Math::Vector3 *vertices;
  std::vector<i32> indices;
  Math::Vector3 *normals;
  Math::Vector3 *uv;

private:
  std::vector<Math::Vector3> _vertices;
  std::vector<Math::Vector3> _normals;
  std::vector<Math::Vector2> _uv;
};
} // namespace Mesh
