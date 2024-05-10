/**
 * @file renderer.hpp
 *
 * @brief header file for renderer class
 *
 * @details Implements the renderer using Vulkan.
 */

#pragma once

#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Renderer {
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class Renderer {
public:
private:
  struct Shaders {
    VkShaderModule vertex = nullptr;
    VkShaderModule fragment = nullptr;
    VkShaderModule tesselation = nullptr;
    VkShaderModule geometery = nullptr;
  } shaders;
  void createShaders(std::string vertexFilePath, std::string fragmentFilePath,
                     std::string tesselationFilePath = nullptr,
                     std::string geometeryFilePath = nullptr);

  VkShaderModule createShaderModule(std::vector<char> &shader);

  void createPipeline();

  void createTexture();

  void createVertexBuffer();

  void createUniform();

  void draw();

  // Instance Variables
  VkDevice device;
};
} // namespace Renderer
