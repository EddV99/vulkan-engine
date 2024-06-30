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

  void init(GLFWwindow *window, uint32_t width, uint32_t height);

  void initializeVulkan();
  void createAssets(Game::Scene &scene);
  void createPipeline();

  /**
   * Render the whole scene
   */
  void drawScene();
  /**
   * Set to resize when possible
   */
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
  };

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
    alignas(16) Math::Matrix3 mvn;
  };

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

  void createTextureImage(void *textureData, int width, int height, VkImage &image, VkDeviceMemory &imageMemory);

  void createTextureImageView(VkImage &image, VkImageView &imageView);

  void createTextureSampler(VkSampler &sampler);

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
  void createTexture(void *textureData, int width, int height, VkImage &image, VkDeviceMemory &imageMemory,
                     VkImageView &imageView, VkSampler sampler);

  void setupCubemap();

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

  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

  uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

  VkDeviceSize getMinUniformBufferOffsetAlignment();

  VkDeviceSize getUniformBufferAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

  /**
   * Perspective Matrix
   */
  Math::Matrix4 perspectiveMatrix(f32 fov, f32 aspect);

  /**
   * Instance Variables
   */
  GLFWwindow *window;
  VkInstance instance;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  QueueFamily queueFamily;
  VkDevice device;
  VkSurfaceKHR surface;
  VkSwapchainKHR swapchain;
  VkExtent2D swapchainExtent;
  std::vector<VkImageView> swapchainImageViews;
  std::vector<VkImage> swapchainImages;
  VkFormat swapchainImageFormat;
  VkExtent2D extent;

  VkRenderPass colorAndDepthRenderPass;
  VkRenderPass depthRenderPass;

  // describes uniform variables
  VkDescriptorSetLayout descriptorSetLayout;
  // references descriptor set, push constants
  VkPipelineLayout pipelineLayout;
  // references the pipeline layout and renderpass
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
  VkDescriptorPool descriptorPool;
  std::vector<VkDescriptorSet> descriptorSets;

  /**
   * Depth needed for 3D rendering
   */
  VkImage depthImage;
  VkDeviceMemory depthImageMemory;
  VkImageView depthImageView;

  /**
   * Vertex data
   *  (we'll combine scene mesh data into one big buffer)
   */
  VkBuffer meshBuffer;
  VkDeviceMemory meshMemory;
  VkBuffer indexBuffer;
  VkDeviceMemory indexMemory;
  // data needed to be able to index into individual models from our big mesh buffer
  std::vector<int32_t> vertexOffsets;
  std::vector<uint32_t> indexOffsets;
  std::vector<uint32_t> indexCount;

  /**
   * Uniform buffers
   */
  std::vector<VkBuffer> uniformBuffers;
  std::vector<VkDeviceMemory> uniformBuffersMemory;
  std::vector<void *> uniformBuffersMapped;
  std::vector<UniformBufferObject> ubos;

  VkDeviceSize alignment;
  VkDeviceSize dynamicUniformBufferSize;

  /**
   * Texture
   */
  std::vector<VkImage> textureImage;
  std::vector<VkDeviceMemory> textureImageMemory;
  std::vector<VkImageView> textureImageView;
  std::vector<VkSampler> textureSampler;

  /**
   * cubemap
   *  (should just be one)
   */
  VkImage cubemapImage;
  VkDeviceMemory cubemapImageMemory;
  VkImageView cubemapImageView;
  VkSampler cubemapSampler;

  // when no textures are needed send a "dummy" texture
  unsigned char DEFAULT_IMAGE[4] = {0, 0, 0, 0};

  // Shaders for pipeline
  Shaders shaders;

  // one triangle for cubemap
  f32 triangleVertices[9] = {-1.0, -1.0, 0.999, //
                             3.0,  -1.0, 0.999, //
                             -1.0, 3.0,  0.999};

  /**
   * Scene to render
   */
  Game::Scene *scene;

  /**
   * List of wanted validation layers
   */
  const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

  const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  /**
   * Values for perspective matrix
   */
  // far/near
  f32 f = 100;
  f32 n = 0.1;
  // top/bottom
  f32 t;
  f32 b;
  // right/left
  f32 r;
  f32 l;

  /**
   * Width and Height
   */
  uint32_t WIDTH;
  uint32_t HEIGHT;

  /**
   * Number of objects to render
   */
  size_t OBJECT_COUNT = 0;

  /**
   * Number of frames in flight at a time (2 is double buffering)
   */
  const int MAX_FRAMES_IN_FLIGHT = 2;

  /**
   * Enable validation layers in debug mode
   */
#ifdef NDEBUG
  const bool enableValidationLayers = false;
#else
  const bool enableValidationLayers = true;
#endif
};

} // namespace Renderer
