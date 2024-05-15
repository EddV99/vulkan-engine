/**
 * @file renderer.cpp
 */

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "../util/util.hpp"
#include "renderer.hpp"

#include <cstring>

namespace Renderer {

void Renderer::init() {
  if (enableValidationLayers && !checkValidationLayerSupport()) {
    Util::Error("Validation layers requested, but failed to get");
  }
  createWindow();
  createInstance();
}
void Renderer::createWindow() {
  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window = glfwCreateWindow(WIDTH, HEIGHT, "Beast Gohan", nullptr, nullptr);
}

void Renderer::createInstance() {
  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Beast Gohan";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "Double-D";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  createInfo.enabledExtensionCount = glfwExtensionCount;
  createInfo.ppEnabledExtensionNames = glfwExtensions;

  if (enableValidationLayers) {
    createInfo.enabledLayerCount = (uint32_t)validationLayers.size();
    createInfo.ppEnabledLayerNames = validationLayers.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }

  if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
    Util::Error("Failed to create instance");
  }
}

bool Renderer::checkValidationLayerSupport() {
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  bool layerFound;
  for (const auto &layer : validationLayers) {
    layerFound = false;
    for (const auto &layerProperties : availableLayers) {
      if (strcmp(layer, layerProperties.layerName) == 0) {
        layerFound = true;
        break;
      }
    }
    if (!layerFound)
      return false;
  }
  return true;
}

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
