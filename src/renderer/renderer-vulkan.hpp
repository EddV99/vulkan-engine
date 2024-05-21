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
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "renderer-core.hpp"

namespace Renderer {
// ====================================================================================================================
// Render Vulkan Class
// ====================================================================================================================
class RendererVulkan {
public:
  RendererVulkan() = default;
  RendererVulkan(uint32_t width, uint32_t height);
  ~RendererVulkan();

  void drawFrame();
  void drawFrame(FrameData frame);
  void init(GLFWwindow *window);
  void resize();

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
  };

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

  const std::vector<const char *> deviceExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  /**
   * Vulkan Initilization
   */
  void createInstance();

  void createSurface();

  void pickPhysicalDevice();

  void createDevice();

  void createSwapchain();

  void createImageViews();

  void createRenderPass();

  void createGraphicsPipeline();

  void createFrameBuffers();

  void createCommandBuffer();

  void createSyncObjects();

  /**
   * Helper Methods
   */
  bool checkValidationLayerSupport();

  QueueFamily setupQueueFamilies(VkPhysicalDevice physicalDevice);

  bool isDeviceCompatible(VkPhysicalDevice physicalDevice);

  bool checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice);

  SwapchainSupportDetails

  querySwapchainSupport(VkPhysicalDevice physicalDevice);

  VkSurfaceFormatKHR
  chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &available);

  VkPresentModeKHR
  chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &available);

  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

  void recreateSwapchain();

  void cleanSwapchain();

  void createShaders(std::string vertexFilePath, std::string fragmentFilePath,
                     std::string tesselationFilePath = nullptr,
                     std::string geometeryFilePath = nullptr);

  VkShaderModule createShaderModule(std::vector<char> &shader);

  void createTexture();

  void createVertexBuffer();

  void createUniform();

  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

  /**
   * Instance Variables
   */
  GLFWwindow *window;
  VkInstance instance;
  VkSurfaceKHR surface;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkDevice device;
  QueueFamily queueFamily;
  VkSwapchainKHR swapchain;
  VkExtent2D swapchainExtent;
  std::vector<VkImageView> swapchainImageViews;
  std::vector<VkImage> swapchainImages;
  VkFormat swapchainImageFormat;
  VkRenderPass renderPass;
  VkExtent2D extent;
  VkPipelineLayout pipelineLayout;
  VkPipeline graphicsPipeline;
  std::vector<VkFramebuffer> framebuffers;
  std::vector<VkCommandBuffer> commandBuffers;
  VkCommandPool commandPool;
  std::vector<VkSemaphore> imageAvailableSem;
  std::vector<VkSemaphore> renderFinishedSem;
  std::vector<VkFence> inFlightFence;
  VkQueue graphicsQueue;
  VkQueue presentQueue;
  uint32_t currentFrame = 0;
  bool resized = false;

  /**
   * Constants
   */
  uint32_t WIDTH = 500;
  uint32_t HEIGHT = 500;

  const int MAX_FRAMES_IN_FLIGHT = 2;

#ifdef NDEBUG
  const bool enableValidationLayers = false;
#else
  const bool enableValidationLayers = true;
#endif
};
} // namespace Renderer
