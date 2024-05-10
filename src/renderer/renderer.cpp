/**
 * @file renderer.cpp
 */

#include "renderer.hpp"
#include "../util/util.hpp"
#include <vulkan/vulkan_core.h>

namespace Renderer {

void Renderer::createShaders(std::string vertexFilePath,
                             std::string fragmentFilePath,
                             std::string tesselationFilePath,
                             std::string geometeryFilePath) {

  auto vertexShader = Util::readFile(vertexFilePath);
  auto fragmentShader = Util::readFile(fragmentFilePath);

  VkShaderModule vertex, fragment;
  vertex = createShaderModule(vertexShader);
  fragment = createShaderModule(fragmentShader);

  shaders.vertex = vertex;
  shaders.fragment = fragment;
}

VkShaderModule Renderer::createShaderModule(std::vector<char> &shader) {

  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = shader.size();
  createInfo.pCode = reinterpret_cast<const uint32_t *>(shader.data());

  VkShaderModule module;

  if (vkCreateShaderModule(device, &createInfo, nullptr, &module) !=
      VK_SUCCESS) {
    Util::Error("Failed to create shader module");
  }

  return module;
}

void Renderer::createPipeline() {}

void Renderer::createTexture() {}

void Renderer::createVertexBuffer() {}

void Renderer::createUniform() {}

void Renderer::draw() {}

} // namespace Renderer
