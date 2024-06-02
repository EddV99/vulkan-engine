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
public:
  Mesh() = default;
  Mesh &operator=(const Mesh &other);

  void computeBoundingBox();

  bool hasNormals();
  bool hasUV();

  void loadOBJFile(std::string filename);

  uint32_t size = 0;
  std::vector<u32> indices;
  std::vector<Math::Vector3> vertices;
  std::vector<Math::Vector3> normals;
  std::vector<Math::Vector2> uv;

  // VULKAN ONLY ----------------------------------------------
  VkVertexInputBindingDescription getBindingDescriptions() {
    VkVertexInputBindingDescription result;
    result.binding = 0;
    result.stride = (sizeof(Math::Vector3) * 2) + sizeof(Math::Vector2);
    result.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return result;
  }

  std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
    auto v = getAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0);
    uint32_t offset = vertices.size();
    auto n = getAttributeDescription(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offset);
    offset += normals.size();
    auto u = getAttributeDescription(0, 2, VK_FORMAT_R32G32_SFLOAT, offset);
    std::array<VkVertexInputAttributeDescription, 3> result = {v, n, u};
    return result;
  }
  // VULKAN ONLY END -----------------------------------------

private:
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

  bool _hasNormals{};
  bool _hasUV{};
};

using Scene = std::vector<Mesh>;

} // namespace Mesh
