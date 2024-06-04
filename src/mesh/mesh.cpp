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
Mesh::~Mesh() {
  _hasNormals = false;
  _hasUV = false;

  size = 0;
  indices.clear();
  data.clear();
}
Mesh::Mesh(const Mesh &other)
    : _hasNormals(other._hasNormals), _hasUV(other._hasUV), size(other.size), indices(other.indices), data(other.data) {
}
Mesh::Mesh(Mesh &&other) noexcept
    : _hasNormals(other._hasNormals), _hasUV(other._hasUV), size(other.size), indices(other.indices), data(other.data) {
  other._hasNormals = false;
  other._hasUV = false;

  other.size = 0;
  other.indices.clear();
  other.data.clear();
}
Mesh &Mesh::operator=(Mesh &&other) noexcept {
  if (this == &other)
    return *this;

  this->_hasNormals = other._hasNormals;
  this->_hasUV = other._hasUV;

  this->size = other.size;
  this->indices = other.indices;
  this->data = other.data;

  other._hasNormals = false;
  other._hasUV = false;

  other.size = 0;
  other.indices.clear();
  other.data.clear();

  return *this;
}
Mesh &Mesh::operator=(const Mesh &other) {
  if (this == &other)
    return *this;

  this->_hasNormals = other._hasNormals;
  this->_hasUV = other._hasUV;

  this->size = other.size;
  this->indices = other.indices;
  this->data = other.data;

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
      Data d;
      Math::Vector3 vertex;
      vertex.x = attrib.vertices[3 * index.vertex_index + 0];
      vertex.y = attrib.vertices[3 * index.vertex_index + 1];
      vertex.z = attrib.vertices[3 * index.vertex_index + 2];
      if (uniqueVertices.count(vertex) == 0) {
        uniqueVertices[vertex] = static_cast<uint32_t>(data.size());
        d.vertex = vertex;
        if (index.normal_index >= 0) {
          _hasNormals = true;
          Math::Vector3 normal;
          normal.x = attrib.normals[3 * index.normal_index + 0];
          normal.y = attrib.normals[3 * index.normal_index + 1];
          normal.z = attrib.normals[3 * index.normal_index + 2];

          d.normal = normal;
        }
        if (index.texcoord_index >= 0) {
          _hasUV = true;
          Math::Vector2 tex;
          tex.x = attrib.texcoords[2 * index.texcoord_index + 0];
          tex.y = attrib.texcoords[2 * index.texcoord_index + 1];

          d.uv = tex;
        }

        data.push_back(d);
      }
      indices.push_back(uniqueVertices[vertex]);
    }
  }

  this->size = indices.size();
}

void Mesh::computeBoundingBox() {}

bool Mesh::hasNormals() { return _hasNormals; }
bool Mesh::hasUV() { return _hasUV; }
} // namespace Mesh
