/**
 * @file renderer.hpp
 *
 * @brief header file for renderer class
 *
 * @details Implements the renderer using Vulkan.
 */

#pragma once

#include "renderer-core.hpp"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "../game/scene.hpp"

namespace Renderer {
// ====================================================================================================================
// Render Vulkan Class
// ====================================================================================================================
class RendererVulkan {
public:
  RendererVulkan() = default;
  RendererVulkan(uint32_t width, uint32_t height);
  ~RendererVulkan();

  void drawScene();
  void init(GLFWwindow *window, Game::Scene &scene);
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

  struct UniformBufferObject {
    alignas(16) Math::Matrix4 model;
    alignas(16) Math::Matrix4 view;
    alignas(16) Math::Matrix4 proj;
  } ubo;

  /**
   * List of wanted validation layers
   */
  const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

  const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

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

  void createDescriptorSetLayout();

  void createGraphicsPipeline();

  void createFrameBuffers();

  void createCommandPool();

  void createDepthResources();

  void createTextureImage(void *textureData, int width, int height);

  void createTextureImageView();

  void createTextureSampler();

  void createDescriptorSets();

  void createVertexBuffer(void *vertexData, size_t size);

  void createIndexBuffer(void *indexData, size_t size);

  void createUniformBuffers();

  void createDescriptorPool();

  void createCommandBuffer();

  void createSyncObjects();

  /**
   * Helper Methods
   */
  bool hasStencilComponent(VkFormat format);

  VkFormat findDepthFormat();

  VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling,
                               VkFormatFeatureFlags features);
  VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

  void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

  void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

  VkCommandBuffer beginSingleTimeCommands();

  void endSingleTimeCommands(VkCommandBuffer commandBuffer);

  void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
                   VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemor);

  void updateUniformBuffer(uint32_t frame);

  bool checkValidationLayerSupport();

  void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer,
                    VkDeviceMemory &memory);

  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

  QueueFamily setupQueueFamilies(VkPhysicalDevice physicalDevice);

  bool isDeviceCompatible(VkPhysicalDevice physicalDevice);

  bool checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice);

  SwapchainSupportDetails

  querySwapchainSupport(VkPhysicalDevice physicalDevice);

  VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &available);

  VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &available);

  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

  void recreateSwapchain();

  void cleanSwapchain();

  void createShaders(std::string vertexFilePath, std::string fragmentFilePath, std::string tesselationFilePath = "",
                     std::string geometeryFilePath = "");

  VkShaderModule createShaderModule(std::vector<char> &shader);

  void createTexture();

  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, uint32_t indexCount,
                           uint32_t indexOffset, int32_t vertexOffset);

  uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

  /**
   * Perspective Matrix
   */
  f32 f = 100;
  f32 n = 0.1;

  f32 dt = 0;

  f32 t = 400;
  f32 b = -400;

  f32 r = 400;
  f32 l = -400;
  Math::Matrix4 perspectiveMatrix(f32 fov, f32 aspect);

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
  VkDescriptorSetLayout descriptorSetLayout;
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
  Game::Scene *scene;
  VkDescriptorPool descriptorPool;
  std::vector<VkDescriptorSet> descriptorSets;

  // depth
  VkImage depthImage;
  VkDeviceMemory depthImageMemory;
  VkImageView depthImageView;

  // mesh(s)
  VkBuffer meshBuffer;
  VkDeviceMemory meshMemory;

  VkBuffer indexBuffer;
  VkDeviceMemory indexMemory;

  std::vector<VkBuffer> uniformBuffers;
  std::vector<VkDeviceMemory> uniformBuffersMemory;
  std::vector<void *> uniformBuffersMapped;

  bool madeTextureImage = false;
  VkImage textureImage;
  VkDeviceMemory textureImageMemory;
  VkImageView textureImageView;
  VkSampler textureSampler;

  std::vector<size_t> dataOffsets;
  std::vector<size_t> indexOffsets;
  std::vector<size_t> indexCount;

  /**
   * Constants
   */
  uint32_t WIDTH = 500;
  uint32_t HEIGHT = 500;

  const int MAX_FRAMES_IN_FLIGHT = 2;

  unsigned char DEFAULT_IMAGE[4] = {0, 0, 0, 0};

#ifdef NDEBUG
  const bool enableValidationLayers = false;
#else
  const bool enableValidationLayers = true;
#endif
};

} // namespace Renderer
