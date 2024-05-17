/**
 * @file renderer.hpp
 *
 * @brief header file for renderer class
 *
 * @details Implements the renderer using Vulkan.
 */

#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <optional>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Renderer {

// ====================================================================================================================
//
// ====================================================================================================================
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

// ====================================================================================================================
//
// ====================================================================================================================
class RendererVulkan {
public:
  void draw();
  void init();

private:
  /**
   * Internal Structs
   */
  struct Shaders {
    VkShaderModule vertex = nullptr;
    VkShaderModule fragment = nullptr;
    VkShaderModule tesselation = nullptr;
    VkShaderModule geometery = nullptr;
  } shaders;

  struct QueueFamily {
    std::optional<uint32_t> graphics;
    std::optional<uint32_t> present;

    bool compatible() { return graphics.has_value() && present.has_value(); }
  } queueFamily;

  struct SwapchainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
  } swapchainSupport;

  /**
   * List of wanted validation layers
   */
  const std::vector<const char *> validationLayers = {
      "VK_LAYER_KHRONOS_validation"};

  /**
   * Function Declarations
   */
  bool checkValidationLayerSupport();

  void createWindow();

  void createInstance();

  void createShaders(std::string vertexFilePath, std::string fragmentFilePath,
                     std::string tesselationFilePath = nullptr,
                     std::string geometeryFilePath = nullptr);

  VkShaderModule createShaderModule(std::vector<char> &shader);

  void createPipeline();

  void createTexture();

  void createVertexBuffer();

  void createUniform();

  /**
   * Instance Variables
   */
  VkDevice device;
  VkInstance instance;
  GLFWwindow *window;

  const uint32_t WIDTH = 500;
  const uint32_t HEIGHT = 500;
};
} // namespace Renderer
