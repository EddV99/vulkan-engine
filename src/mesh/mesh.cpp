/**
 * @file mesh.cpp
 */

#define TINYOBJLOADER_IMPLEMENTATION
#include "../util/tiny_obj_loader.h"

#include "../math/vector.hpp"
#include "../util/util.hpp"
#include "mesh.hpp"

#include <string>
#include <unordered_map>

namespace Mesh {
Mesh::~Mesh() {
  indices.clear();
  vertexData.clear();
}
Mesh::Mesh(const Mesh &other)
    : _hasNormals(other._hasNormals), _hasUV(other._hasUV), vertexCount(other.vertexCount),
      vertexDataSize(other.vertexDataSize), indexDataSize(other.indexDataSize), indices(other.indices),
      vertexData(other.vertexData), box(other.box) {}

Mesh::Mesh(Mesh &&other) noexcept
    : _hasNormals(other._hasNormals), _hasUV(other._hasUV), vertexCount(other.vertexCount),
      vertexDataSize(other.vertexDataSize), indexDataSize(other.indexDataSize), indices(other.indices),
      vertexData(other.vertexData), box(other.box) {

  other.indices.clear();
  other.vertexData.clear();
}
Mesh &Mesh::operator=(Mesh &&other) noexcept {
  if (this == &other)
    return *this;

  this->_hasNormals = other._hasNormals;
  this->_hasUV = other._hasUV;

  this->vertexCount = other.vertexCount;
  this->vertexDataSize = other.vertexDataSize;
  this->indexDataSize = other.indexDataSize;
  this->indices = other.indices;
  this->vertexData = other.vertexData;

  other.indices.clear();
  other.vertexData.clear();

  return *this;
}
Mesh &Mesh::operator=(const Mesh &other) {
  if (this == &other)
    return *this;

  this->_hasNormals = other._hasNormals;
  this->_hasUV = other._hasUV;

  this->vertexCount = other.vertexCount;
  this->vertexDataSize = other.vertexDataSize;
  this->indexDataSize = other.indexDataSize;
  this->indices = other.indices;
  this->vertexData = other.vertexData;

  return *this;
}

void Mesh::init(std::string meshPath) {
  loadOBJFile(meshPath);
  computeBoundingBox();
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

  std::unordered_map<Math::Vector3, u32> uniqueVertices{};

  _hasNormals = false;
  _hasUV = false;
  for (const auto &shape : shapes) {
    for (const auto &index : shape.mesh.indices) {
      Vertex d;
      Math::Vector3 vertex;
      vertex.x = attrib.vertices[3 * index.vertex_index + 0];
      vertex.y = attrib.vertices[3 * index.vertex_index + 1];
      vertex.z = attrib.vertices[3 * index.vertex_index + 2];
      if (uniqueVertices.count(vertex) == 0) {
        uniqueVertices[vertex] = static_cast<uint32_t>(vertexData.size());
        d.position = vertex;
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

        vertexData.push_back(d);
      }
      indices.push_back(uniqueVertices[vertex]);
    }
  }
  vertexCount = indices.size();
  vertexDataSize = vertexData.size() * sizeof(Vertex);
  indexDataSize = indices.size() * sizeof(indices[0]);
}

void Mesh::computeBoundingBox() {
  Math::Vector3 v = vertexData[0].position;
  box.max = v;
  box.min = v;
  box.mid = {0, 0, 0};

  for (Vertex d : vertexData) {
    v = d.position;

    if (box.max.x < v.x)
      box.max.x = v.x;

    if (box.max.y < v.y)
      box.max.y = v.y;

    if (box.max.z < v.z)
      box.max.z = v.z;

    if (box.min.x > v.x)
      box.min.x = v.x;

    if (box.min.y > v.y)
      box.min.y = v.y;

    if (box.min.z > v.z)
      box.min.z = v.z;
  }

  box.mid = (box.max + box.min) * 0.5;
}

bool Mesh::hasNormals() { return _hasNormals; }
bool Mesh::hasUV() { return _hasUV; }

const std::vector<u32> &Mesh::getIndices() const { return indices; }
const std::vector<Vertex> &Mesh::getVertexData() const { return vertexData; }
const Mesh::BoundingBox &Mesh::getBoundingBox() const { return box; }

size_t Mesh::getVertexCount() const { return vertexCount; }
size_t Mesh::getVertexDataSize() const { return vertexDataSize; }
size_t Mesh::getIndexDataSize() const { return indexDataSize; }

} // namespace Mesh
