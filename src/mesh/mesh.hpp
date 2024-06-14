/**
 * @file mesh.hpp
 *
 * @brief header file for mesh class
 *
 */
#pragma once

#include "../math/vector.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Mesh {
class Mesh {
private:
  struct Vertex {
    Math::Vector3 position;
    Math::Vector3 normal;
    Math::Vector2 uv;
  };

  struct BoundingBox {
    Math::Vector3 max;
    Math::Vector3 min;
    Math::Vector3 mid;
  };

  // VULKAN ONLY ----------------------------------------------
  static VkVertexInputAttributeDescription getAttributeDescription(uint32_t bind, uint32_t loc, VkFormat format,
                                                                   uint32_t offset) {
    VkVertexInputAttributeDescription attributeDescriptions{};

    attributeDescriptions.binding = bind;
    attributeDescriptions.location = loc;
    attributeDescriptions.format = format;
    attributeDescriptions.offset = offset;

    return attributeDescriptions;
  }
  // VULKAN ONLY END -----------------------------------------

  bool _hasNormals = false;
  bool _hasUV = false;
  size_t vertexCount = 0;
  size_t vertexDataSize = 0, indexDataSize = 0;
  std::vector<u32> indices;
  std::vector<Vertex> vertexData;
  BoundingBox box;
  void loadOBJFile(std::string filename);

public:
  Mesh() = default;
  Mesh(const Mesh &other);
  Mesh(Mesh &&other) noexcept;
  ~Mesh();
  Mesh &operator=(const Mesh &other);
  Mesh &operator=(Mesh &&other) noexcept;

  void init(std::string meshPath);

  void computeBoundingBox();
  bool hasNormals();
  bool hasUV();

  const std::vector<u32> &getIndices();
  const std::vector<Vertex> &getVertexData();
  const BoundingBox &getBoundingBox();

  size_t getVertexCount();
  size_t getVertexDataSize();
  size_t getIndexDataSize();

  // VULKAN ONLY ----------------------------------------------
  VkVertexInputBindingDescription getBindingDescriptions() {
    VkVertexInputBindingDescription result;
    result.binding = 0;
    result.stride = sizeof(Vertex);
    result.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return result;
  }

  std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
    auto v = getAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position));
    auto n = getAttributeDescription(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal));
    auto u = getAttributeDescription(0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv));

    std::array<VkVertexInputAttributeDescription, 3> result = {v, n, u};
    return result;
  }
  // VULKAN ONLY END -----------------------------------------
};
} // namespace Mesh
