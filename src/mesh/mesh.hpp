/**
 * @file mesh.hpp
 *
 * @brief header file for mesh class
 *
 */
#pragma once

#include "../math/vector.hpp"

#include <array>
#include <cstdint>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Mesh {
class Mesh {
public:
  Mesh() = default;

  void computeBoundingBox();

  bool hasNormals();
  bool hasUV();

  void loadOBJFile(std::string filename);

  Math::Vector3 *vertices = nullptr;
  std::vector<i32> indices;
  Math::Vector3 *normals = nullptr;
  Math::Vector3 *uv = nullptr;

  // VULKAN ONLY ----------------------------------------------
  static VkVertexInputBindingDescription getBindingDescriptionVertex() {
    VkVertexInputBindingDescription description;
    description.binding = 0;
    description.stride = sizeof(Math::Vector3);
    description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return description;
  }

  static VkVertexInputBindingDescription getBindingDescriptionNormal() {
    VkVertexInputBindingDescription description;
    description.binding = 1;
    description.stride = sizeof(Math::Vector3);
    description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return description;
  }

  static VkVertexInputBindingDescription getBindingDescriptionUV() {
    VkVertexInputBindingDescription description;
    description.binding = 2;
    description.stride = sizeof(Math::Vector2);
    description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return description;
  }

  static VkVertexInputAttributeDescription getAttributeDescription(uint32_t bind, uint32_t loc, VkFormat format) {
    VkVertexInputAttributeDescription attributeDescriptions{};

    attributeDescriptions.binding = bind;
    attributeDescriptions.location = loc;
    attributeDescriptions.format = format;
    attributeDescriptions.offset = 0;

    return attributeDescriptions;
  }
  // VULKAN ONLY END -----------------------------------------

private:
  std::vector<Math::Vector3> _vertices;
  std::vector<Math::Vector3> _normals;
  std::vector<Math::Vector2> _uv;
  bool _hasNormals{};
  bool _hasUV{};
};
} // namespace Mesh
