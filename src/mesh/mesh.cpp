/**
 * @file mesh.cpp
 */

#include "mesh.hpp"
#include "../math/vector.hpp"
#include "../util/tiny_obj_loader.h"
#include "../util/util.hpp"

#include <cstring>
#include <string>
#include <unordered_map>

namespace Mesh {

void Mesh::loadOBJFile(std::string filename) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;

  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                        filename.c_str())) {
    std::string warnMessage{};
    std::string errMessage{};
    if (!warn.empty())
      warnMessage = "WARNING: " + warn;

    if (!err.empty())
      errMessage = "ERROR: " + err;
    Util::Error(warnMessage + " " + errMessage);
  }

  std::unordered_map<Math::Vector3, i32> uniqueVertices{};
  bool hasNormals = false;
  bool hasUV = false;

  for (const auto &shape : shapes) {
    for (const auto &index : shape.mesh.indices) {
      Math::Vector3 vertex;
      vertex.x = attrib.vertices[3 * index.vertex_index + 0];
      vertex.y = attrib.vertices[3 * index.vertex_index + 1];
      vertex.z = attrib.vertices[3 * index.vertex_index + 2];

      if (uniqueVertices.count(vertex) == 0) {
        uniqueVertices[vertex] = static_cast<uint32_t>(_vertices.size());
        _vertices.push_back(vertex);

        if (index.normal_index > 0) {
          hasNormals = true;
          Math::Vector3 normal;
          normal.x = attrib.normals[3 * index.normal_index + 0];
          normal.y = attrib.normals[3 * index.normal_index + 1];
          normal.z = attrib.normals[3 * index.normal_index + 2];
          _normals.push_back(normal);
        }
        if (index.texcoord_index > 0) {
          hasUV = true;
          Math::Vector2 tex;
          tex.x = attrib.texcoords[2 * index.texcoord_index + 0];
          tex.y = attrib.texcoords[2 * index.texcoord_index + 1];
          _uv.push_back(tex);
        }
      }
      indices.push_back(uniqueVertices[vertex]);
    }
  }

  memcpy(vertices, _vertices.data(), sizeof(Math::Vector3) * _vertices.size());
  if (hasNormals)
    memcpy(normals, _normals.data(), sizeof(Math::Vector3) * _normals.size());
  if (hasUV)
    memcpy(uv, _uv.data(), sizeof(Math::Vector2) * _uv.size());
}
} // namespace Mesh
