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
  struct Data {
    Math::Vector3 vertex;
    Math::Vector3 normal;
    Math::Vector2 uv;
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

  bool _hasNormals{};
  bool _hasUV{};

public:
  Mesh() = default;
  Mesh &operator=(const Mesh &other);

  void computeBoundingBox();

  bool hasNormals();
  bool hasUV();

  void loadOBJFile(std::string filename);

  uint32_t size = 0;
  std::vector<u32> indices;
  std::vector<Data> data;

  // VULKAN ONLY ----------------------------------------------
  VkVertexInputBindingDescription getBindingDescriptions() {
    VkVertexInputBindingDescription result;
    result.binding = 0;
    result.stride = sizeof(Data);
    result.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return result;
  }

  std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
    auto v = getAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Data, vertex));
    auto n = getAttributeDescription(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Data, normal));
    auto u = getAttributeDescription(0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(Data, uv));

    std::array<VkVertexInputAttributeDescription, 3> result = {v, n, u};
    return result;
  }
  // VULKAN ONLY END -----------------------------------------
};

using Scene = std::vector<Mesh>;

} // namespace Mesh
