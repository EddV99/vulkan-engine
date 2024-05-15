/**
 * @file mesh.hpp
 *
 * @brief header file for mesh class
 *
 */
#pragma once

#include "../math/vector.hpp"
#include <vector>

namespace Mesh {
class Mesh {
public:
  Mesh() = default;
  Mesh(std::vector<float> vertices, std::vector<float> normals = {},
       std::vector<float> uv = {});

  void computeBoundingBox();

  bool hasNormals();
  bool hasUV();

  std::vector<Math::Vector3> vertices;
  std::vector<Math::Vector3> normals;
  std::vector<Math::Vector3> uv;

private:
};
} // namespace Mesh
