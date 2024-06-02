/**
 * @file mesh.cpp
 */

#include "mesh.hpp"
#include "../math/vector.hpp"
#include <cstdlib>
#define TINYOBJLOADER_IMPLEMENTATION
#include "../util/tiny_obj_loader.h"
#include "../util/util.hpp"

#include <cstring>
#include <string>
#include <unordered_map>

namespace Mesh {

Mesh &Mesh::operator=(const Mesh &other) {
  this->uv = other.uv;
  this->normals = other.normals;
  this->vertices = other.vertices;

  this->_hasUV = other._hasUV;
  this->_hasNormals = other._hasNormals;
  return *this;
}

void Mesh::loadOBJFile(std::string filename) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;

  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str())) {
    std::string warnMessage;
    std::string errMessage;
    if (!warn.empty())
      warnMessage = "Mesh::loadOBJFile Warning: " + warn;

    if (!err.empty())
      errMessage = "Mesh::loadOBJFile Error: " + err;

    Util::Error(warnMessage + " " + errMessage);
  }

  std::unordered_map<Math::Vector3, i32> uniqueVertices{};

  _hasNormals = false;
  _hasUV = false;

  for (const auto &shape : shapes) {
    for (const auto &index : shape.mesh.indices) {
      Math::Vector3 vertex;
      vertex.x = attrib.vertices[3 * index.vertex_index + 0];
      vertex.y = attrib.vertices[3 * index.vertex_index + 1];
      vertex.z = attrib.vertices[3 * index.vertex_index + 2];

      if (uniqueVertices.count(vertex) == 0) {
        uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
        vertices.push_back(vertex);

        if (index.normal_index >= 0) {
          _hasNormals = true;
          Math::Vector3 normal;
          normal.x = attrib.normals[3 * index.normal_index + 0];
          normal.y = attrib.normals[3 * index.normal_index + 1];
          normal.z = attrib.normals[3 * index.normal_index + 2];
          normals.push_back(normal);
        }
        if (index.texcoord_index >= 0) {
          _hasUV = true;
          Math::Vector2 tex;
          tex.x = attrib.texcoords[2 * index.texcoord_index + 0];
          tex.y = attrib.texcoords[2 * index.texcoord_index + 1];
          uv.push_back(tex);
        }
      }
      indices.push_back(uniqueVertices[vertex]);
    }
  }

  this->size = indices.size();
}

bool Mesh::hasNormals() { return _hasNormals; }
bool Mesh::hasUV() { return _hasUV; }
} // namespace Mesh
