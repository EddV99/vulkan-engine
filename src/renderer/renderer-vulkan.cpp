/**
 * @file renderer.cpp
 */

#include "renderer-vulkan.hpp"
#include "../util/defines.hpp"
#include "../util/util.hpp"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>

namespace Renderer {

RendererVulkan::RendererVulkan(uint32_t width, uint32_t height) {
  WIDTH = width;
  HEIGHT = height;
  r = (f32)WIDTH / 2.0;
  l = -r;

  t = (f32)HEIGHT / 2.0;
  b = -t;
}

RendererVulkan::~RendererVulkan() {
  vkDeviceWaitIdle(device);

  cleanSwapchain();

  for (size_t i = 0; i < textureSampler.size(); i++) {
    vkDestroySampler(device, textureSampler[i], nullptr);
    vkDestroyImageView(device, textureImageView[i], nullptr);

    vkDestroyImage(device, textureImage[i], nullptr);
    vkFreeMemory(device, textureImageMemory[i], nullptr);
  }

  for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroyBuffer(device, blinn.uniformBuffers[i], nullptr);
    vkFreeMemory(device, blinn.uniformBuffersMemory[i], nullptr);
  }

  vkDestroyDescriptorPool(device, blinn.descriptorPool, nullptr);
  vkDestroyDescriptorSetLayout(device, blinn.descriptorSetLayout, nullptr);

  vkDestroyBuffer(device, indexBuffer, nullptr);
  vkFreeMemory(device, indexMemory, nullptr);

  vkDestroyBuffer(device, meshBuffer, nullptr);
  vkFreeMemory(device, meshMemory, nullptr);

  for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(device, imageAvailableSem[i], nullptr);
    vkDestroySemaphore(device, renderFinishedSem[i], nullptr);
    vkDestroyFence(device, inFlightFence[i], nullptr);
  }

  vkDestroyCommandPool(device, commandPool, nullptr);

  vkDestroyPipeline(device, blinn.pipeline, nullptr);
  vkDestroyPipelineLayout(device, blinn.pipelineLayout, nullptr);
  vkDestroyRenderPass(device, colorAndDepthRenderPass, nullptr);

  vkDestroyDevice(device, nullptr);
  vkDestroySurfaceKHR(instance, surface, nullptr);
  vkDestroyInstance(instance, nullptr);

  glfwDestroyWindow(window);
  glfwTerminate();
}

// ====================================================================================================================
//     Initialization
// ====================================================================================================================
void RendererVulkan::init(GLFWwindow *window, uint32_t width, uint32_t height) {
  if (enableValidationLayers && !checkValidationLayerSupport())
    Util::Error("Validation layers requested, but failed to get");

  this->window = window;

  WIDTH = width;
  HEIGHT = height;

  r = (f32)WIDTH / 2.0;
  l = -r;

  t = (f32)HEIGHT / 2.0;
  b = -t;

  initializeVulkan();
  minUniformSize = getMinUniformBufferOffsetAlignment();
}

void RendererVulkan::initializeVulkan() {
  createInstance();
  createSurface();
  pickPhysicalDevice();
  createDevice();
  createSwapchain();
  createImageViews();
  createRenderPass();
  createCommandPool();
  createCommandBuffer();
  createDepthResources();
  createFrameBuffers();
  createSyncObjects();
}

void RendererVulkan::createAssets(Game::Scene &scene) {
  this->scene = &scene;

  OBJECT_COUNT = scene.objects.size();
  blinn.ubos.resize(OBJECT_COUNT);

  // Combine all data into one big buffer.
  // This provides good data locality.
  size_t vertexDataSize = 0;
  size_t indexDataSize = 0;
  uint32_t offsetCount = 0;
  int32_t vertexCount = 0;
  std::vector<Mesh::Vertex> vertexData;
  std::vector<u32> indexData;

  for (auto &obj : scene.objects) {
    auto v = obj.getMesh().getVertexData();
    auto i = obj.getMesh().getIndices();

    vertexData.insert(vertexData.end(), v.begin(), v.end());
    indexData.insert(indexData.end(), i.begin(), i.end());

    vertexDataSize += obj.getMesh().getVertexDataSize();
    indexDataSize += obj.getMesh().getIndexDataSize();

    indexOffsets.push_back(offsetCount);
    indexCount.push_back((uint32_t)obj.getMesh().getVertexCount());
    offsetCount += (uint32_t)obj.getMesh().getVertexCount();

    vertexOffsets.push_back(vertexCount);
    vertexCount += (int32_t)v.size();
  }

  createVertexBuffer(vertexData.data(), vertexDataSize);
  createIndexBuffer(indexData.data(), indexDataSize);

  // upload textures for objects
  size_t textureCount = scene.getTextureCount();

  if (textureCount == 0) {
    textureCount = 1;
    textureImage.resize(textureCount);
    textureImageMemory.resize(textureCount);
    textureImageView.resize(textureCount);
    textureSampler.resize(textureCount);

    createTextureImage(DEFAULT_IMAGE, 1, 1, textureImage[0], textureImageMemory[0]);
    createTextureImageView(textureImage[0], textureImageView[0]);
    createTextureSampler(textureSampler[0]);
  } else {
    textureImage.resize(textureCount);
    textureImageMemory.resize(textureCount);
    textureImageView.resize(textureCount);
    textureSampler.resize(textureCount);

    size_t i = 0;
    for (auto &obj : scene.objects) {
      if (obj.hasTexture()) {
        auto t = obj.getTextureData();
        createTextureImage(t.pixels, t.width, t.height, textureImage[i], textureImageMemory[i]);
        createTextureImageView(textureImage[i], textureImageView[i]);
        createTextureSampler(textureSampler[i]);
      }
      i++;
    }
  }
}

void RendererVulkan::createPipelines() {
  // Blinn Shading Setup
  blinn.vertexShaderPath = "src/shaders/blinn-vertex.spv";
  blinn.fragmentShaderPath = "src/shaders/blinn-fragment.spv";

  VkDescriptorSetLayoutBinding uniformBindingBlinn{};
  uniformBindingBlinn.binding = 0;
  uniformBindingBlinn.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
  uniformBindingBlinn.descriptorCount = 1;
  uniformBindingBlinn.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  uniformBindingBlinn.pImmutableSamplers = nullptr;

  VkDescriptorSetLayoutBinding samplerBindingBlinn{};
  samplerBindingBlinn.binding = 1;
  samplerBindingBlinn.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerBindingBlinn.descriptorCount = 1;
  samplerBindingBlinn.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  samplerBindingBlinn.pImmutableSamplers = nullptr;

  blinn.layoutBindings = {uniformBindingBlinn, samplerBindingBlinn};

  blinn.descriptorPoolSize.resize(2);
  blinn.descriptorPoolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
  blinn.descriptorPoolSize[0].descriptorCount = (uint32_t)MAX_FRAMES_IN_FLIGHT;

  blinn.descriptorPoolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  blinn.descriptorPoolSize[1].descriptorCount = (uint32_t)MAX_FRAMES_IN_FLIGHT;

  blinn.uniformObjectSize = sizeof(UniformBufferObject);

  blinn.attributeDescriptions = Mesh::Mesh::getAttributeDescriptions();
  blinn.bindingDescription = Mesh::Mesh::getBindingDescriptions();

  createDescriptorSetLayout(blinn);
  createGraphicsPipeline(blinn);
  createUniformBuffers(scene->objects.size(), blinn);
  createDescriptorPool(blinn);
  createDescriptorSets(blinn);

  // after creating descriptor sets you bind them to the uniform buffers
  for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = blinn.uniformBuffers[i];
    bufferInfo.offset = 0;
    bufferInfo.range = blinn.uniformObjectSize;

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = textureImageView[0];
    imageInfo.sampler = textureSampler[0];

    std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = blinn.descriptorSets[i];
    descriptorWrites[0].dstBinding = 0;
    descriptorWrites[0].dstArrayElement = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pBufferInfo = &bufferInfo;
    descriptorWrites[0].pImageInfo = nullptr;
    descriptorWrites[0].pTexelBufferView = nullptr;

    descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet = blinn.descriptorSets[i];
    descriptorWrites[1].dstBinding = 1;
    descriptorWrites[1].dstArrayElement = 0;
    descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].pBufferInfo = nullptr;
    descriptorWrites[1].pImageInfo = &imageInfo;
    descriptorWrites[1].pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(device, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
  }
}

void RendererVulkan::createInstance() {
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

  if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
    Util::Error("Failed to create instance");
}

void RendererVulkan::pickPhysicalDevice() {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

  if (deviceCount <= 0)
    Util::Error("No GPU with vulkan support");

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

  for (const auto &device : devices) {
    if (isDeviceCompatible(device)) {
      physicalDevice = device;
      break;
    }
  }

  if (physicalDevice == VK_NULL_HANDLE)
    Util::Error("No GPU is suitable");

  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(physicalDevice, &properties);
  std::cout << "---------------- PHYSICAL DEVICE SELECTED ----------------\n";
  std::cout << "Device Name: " << properties.deviceName << "\n";
  std::cout << "Vendor ID: " << properties.vendorID << "\n";
  std::cout << "Max Width: " << properties.limits.maxFramebufferWidth << "\n";
  std::cout << "Max Height: " << properties.limits.maxFramebufferHeight << "\n";
  std::cout << "----------------------------------------------------------\n";
}

void RendererVulkan::createDevice() {
  queueFamily = setupQueueFamilies(physicalDevice);

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> unique = {queueFamily.graphics.value(), queueFamily.present.value()};
  float priority = 1.0f;

  for (const auto &family : unique) {
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = family;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &priority;
    queueCreateInfos.push_back(queueCreateInfo);
  }

  VkPhysicalDeviceFeatures deviceFeatures{};
  deviceFeatures.samplerAnisotropy = VK_TRUE;

  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.pQueueCreateInfos = queueCreateInfos.data();
  createInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();
  createInfo.pEnabledFeatures = &deviceFeatures;
  createInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
  createInfo.ppEnabledExtensionNames = deviceExtensions.data();

  // for back compatibility
  if (enableValidationLayers) {
    createInfo.enabledLayerCount = (uint32_t)validationLayers.size();
    createInfo.ppEnabledLayerNames = validationLayers.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }

  if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
    Util::Error("Failed to create logical device");

  // get graphics queue handle
  vkGetDeviceQueue(device, queueFamily.graphics.value(), 0, &graphicsQueue);
  // get present queue handle
  vkGetDeviceQueue(device, queueFamily.present.value(), 0, &presentQueue);
}

void RendererVulkan::createSwapchain() {
  SwapchainSupportDetails swapchainSupport = querySwapchainSupport(physicalDevice);
  VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapchainSupport.formats);
  VkPresentModeKHR presentMode = chooseSwapPresentMode(swapchainSupport.presentModes);

  extent = chooseSwapExtent(swapchainSupport.capabilities);

  uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;

  if (swapchainSupport.capabilities.maxImageCount > 0 && imageCount > swapchainSupport.capabilities.maxImageCount)
    imageCount = swapchainSupport.capabilities.maxImageCount;

  VkSwapchainCreateInfoKHR swapchainCreateInfo{};
  swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchainCreateInfo.surface = surface;
  swapchainCreateInfo.minImageCount = imageCount;
  swapchainCreateInfo.imageFormat = surfaceFormat.format;
  swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
  swapchainCreateInfo.imageExtent = extent;
  swapchainCreateInfo.imageArrayLayers = 1;
  swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  uint32_t indices[] = {queueFamily.graphics.value(), queueFamily.present.value()};

  if (queueFamily.graphics != queueFamily.present) {
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    swapchainCreateInfo.queueFamilyIndexCount = 2;
    swapchainCreateInfo.pQueueFamilyIndices = indices;
  } else {
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.queueFamilyIndexCount = 0;
    swapchainCreateInfo.pQueueFamilyIndices = nullptr;
  }
  swapchainCreateInfo.preTransform = swapchainSupport.capabilities.currentTransform;
  swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchainCreateInfo.presentMode = presentMode;
  swapchainCreateInfo.clipped = VK_TRUE;
  swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain) != VK_SUCCESS)
    Util::Error("Failed to create swapchain");

  vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
  swapchainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages.data());
  swapchainImageFormat = surfaceFormat.format;
  swapchainExtent = extent;
}

void RendererVulkan::createImageViews() {
  swapchainImageViews.resize(swapchainImages.size());
  for (size_t i = 0; i < swapchainImages.size(); i++)
    swapchainImageViews[i] = createImageView(swapchainImages[i], swapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
}

void RendererVulkan::createRenderPass() {
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = swapchainImageFormat;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef{};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription depthAttachment{};
  depthAttachment.format = findDepthFormat();
  depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthAttachmentRef{};
  depthAttachmentRef.attachment = 1;
  depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;
  subpass.pDepthStencilAttachment = &depthAttachmentRef;

  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

  std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
  VkRenderPassCreateInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = (uint32_t)attachments.size();
  renderPassInfo.pAttachments = attachments.data();
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &colorAndDepthRenderPass) != VK_SUCCESS)
    Util::Error("Failed to create render pass");
}

void RendererVulkan::createDescriptorSetLayout(Pipeline &pipeline) {
  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = (uint32_t)pipeline.layoutBindings.size();
  layoutInfo.pBindings = pipeline.layoutBindings.data();

  if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &pipeline.descriptorSetLayout) != VK_SUCCESS)
    Util::Error("Failed to create descriptor set layout");
}

void RendererVulkan::createGraphicsPipeline(Pipeline &pipeline) {
  auto vertexShader = Util::readFile(pipeline.vertexShaderPath);
  auto fragmentShader = Util::readFile(pipeline.fragmentShaderPath);

  VkShaderModule vertexShaderModule = createShaderModule(vertexShader);
  VkShaderModule fragmentShaderModule = createShaderModule(fragmentShader);

  VkPipelineShaderStageCreateInfo vertexShaderStageInfo{};
  vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertexShaderStageInfo.module = vertexShaderModule;
  vertexShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{};
  fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragmentShaderStageInfo.module = fragmentShaderModule;
  fragmentShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo shaderStages[] = {vertexShaderStageInfo, fragmentShaderStageInfo};

  // dynamic part of pipeline
  std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamicState{};
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.dynamicStateCount = (uint32_t)dynamicStates.size();
  dynamicState.pDynamicStates = dynamicStates.data();

  // vertex input
  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = 1;
  vertexInputInfo.pVertexBindingDescriptions = &pipeline.bindingDescription;
  vertexInputInfo.vertexAttributeDescriptionCount = pipeline.attributeDescriptions.size();
  vertexInputInfo.pVertexAttributeDescriptions = pipeline.attributeDescriptions.data();

  // input assembly
  VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
  inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  // viewport stuff
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)extent.width;
  viewport.height = (float)extent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = extent;

  VkPipelineViewportStateCreateInfo viewportState{};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.pViewports = &viewport;
  viewportState.scissorCount = 1;
  viewportState.pScissors = &scissor;

  // rasterizer settings
  VkPipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;
  rasterizer.depthBiasConstantFactor = 0.0f;
  rasterizer.depthBiasClamp = 0.0f;
  rasterizer.depthBiasSlopeFactor = 0.0f;

  // multisampling settings (for now disabled)
  VkPipelineMultisampleStateCreateInfo multisampling{};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling.minSampleShading = 1.0f;
  multisampling.pSampleMask = nullptr;
  multisampling.alphaToCoverageEnable = VK_FALSE;
  multisampling.alphaToOneEnable = VK_FALSE;

  // depth/stencil testing
  VkPipelineDepthStencilStateCreateInfo depthStencil{};
  depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencil.depthTestEnable = pipeline.depthTest;
  depthStencil.depthWriteEnable = pipeline.depthTest;
  depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
  depthStencil.depthBoundsTestEnable = VK_FALSE;
  depthStencil.minDepthBounds = 0.0f;
  depthStencil.maxDepthBounds = 1.0f;
  depthStencil.stencilTestEnable = VK_FALSE;
  depthStencil.front = {};
  depthStencil.back = {};

  // color blending setup
  VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  colorBlendAttachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_FALSE;

  VkPipelineColorBlendStateCreateInfo colorBlending{};
  colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY;
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendAttachment;
  colorBlending.blendConstants[0] = 0.0f;
  colorBlending.blendConstants[1] = 0.0f;
  colorBlending.blendConstants[2] = 0.0f;
  colorBlending.blendConstants[3] = 0.0f;

  // pipeline layout
  // we can set uniform(s) here
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 1;
  pipelineLayoutInfo.pSetLayouts = &pipeline.descriptorSetLayout;
  pipelineLayoutInfo.pushConstantRangeCount = 0;
  pipelineLayoutInfo.pPushConstantRanges = nullptr;

  if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipeline.pipelineLayout) != VK_SUCCESS)
    Util::Error("Failed to create pipeline layout");

  // create the graphics pipeline object
  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = &depthStencil;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = &dynamicState;
  pipelineInfo.layout = pipeline.pipelineLayout;
  if (pipeline.depthTest)
    pipelineInfo.renderPass = colorAndDepthRenderPass;
  else
    pipelineInfo.renderPass = colorAndDepthRenderPass;
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
  pipelineInfo.basePipelineIndex = -1;

  if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline.pipeline) != VK_SUCCESS)
    Util::Error("Failed to create graphics pipeline");

  // destroy modules after setup
  vkDestroyShaderModule(device, vertexShaderModule, nullptr);
  vkDestroyShaderModule(device, fragmentShaderModule, nullptr);
}

void RendererVulkan::createFrameBuffers() {
  int i = 0;
  framebuffers.resize(swapchainImageViews.size());
  for (auto &imageView : swapchainImageViews) {
    std::array<VkImageView, 2> attachments = {imageView, depthImageView};

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = colorAndDepthRenderPass;
    framebufferInfo.attachmentCount = (uint32_t)attachments.size();
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS)
      Util::Error("Failed to create framebuffer");

    i++;
  }
}

void RendererVulkan::createVertexBuffer(void *vertexData, size_t size) {
  VkDeviceSize bufferSize = size;

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingMemory;

  createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
               stagingMemory);

  void *data;
  vkMapMemory(device, stagingMemory, 0, bufferSize, 0, &data);
  memcpy(data, vertexData, (size_t)bufferSize);
  vkUnmapMemory(device, stagingMemory);

  createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, meshBuffer, meshMemory);

  copyBuffer(stagingBuffer, meshBuffer, bufferSize);

  vkDestroyBuffer(device, stagingBuffer, nullptr);
  vkFreeMemory(device, stagingMemory, nullptr);
}

void RendererVulkan::createIndexBuffer(void *indexData, size_t size) {
  VkDeviceSize bufferSize = size;

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingMemory;

  createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
               stagingMemory);

  void *data;
  vkMapMemory(device, stagingMemory, 0, bufferSize, 0, &data);
  memcpy(data, indexData, (size_t)bufferSize);
  vkUnmapMemory(device, stagingMemory);

  createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexMemory);

  copyBuffer(stagingBuffer, indexBuffer, bufferSize);

  vkDestroyBuffer(device, stagingBuffer, nullptr);
  vkFreeMemory(device, stagingMemory, nullptr);
}

void RendererVulkan::createUniformBuffers(size_t objectCount, Pipeline &pipeline) {
  alignment = getUniformBufferAlignment(pipeline.uniformObjectSize, minUniformSize);
  dynamicUniformBufferSize = objectCount * alignment;

  pipeline.uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
  pipeline.uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
  pipeline.uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

  for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    createBuffer(dynamicUniformBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                 pipeline.uniformBuffers[i], pipeline.uniformBuffersMemory[i]);

    vkMapMemory(device, pipeline.uniformBuffersMemory[i], 0, dynamicUniformBufferSize, 0,
                &pipeline.uniformBuffersMapped[i]);
  }
}

void RendererVulkan::createDescriptorPool(Pipeline &pipeline) {
  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = (uint32_t)pipeline.descriptorPoolSize.size();
  poolInfo.pPoolSizes = pipeline.descriptorPoolSize.data();
  poolInfo.maxSets = (uint32_t)MAX_FRAMES_IN_FLIGHT;

  if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &pipeline.descriptorPool) != VK_SUCCESS)
    Util::Error("Failed to create descriptor pool");
}

void RendererVulkan::createDescriptorSets(Pipeline &pipeline) {
  std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, pipeline.descriptorSetLayout);

  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = pipeline.descriptorPool;
  allocInfo.descriptorSetCount = (uint32_t)MAX_FRAMES_IN_FLIGHT;
  allocInfo.pSetLayouts = layouts.data();

  pipeline.descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

  if (vkAllocateDescriptorSets(device, &allocInfo, pipeline.descriptorSets.data()) != VK_SUCCESS)
    Util::Error("Failed to allocate descriptor sets");
}

void RendererVulkan::createCommandPool() {
  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = queueFamily.graphics.value();

  if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
    Util::Error("Failed to create command pool");
}

void RendererVulkan::createDepthResources() {
  VkFormat depthFormat = findDepthFormat();
  createImage(swapchainExtent.width, swapchainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL,
              VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage,
              depthImageMemory);
  depthImageView = createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
  transitionImageLayout(depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED,
                        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

void RendererVulkan::createTextureImage(void *textureData, int width, int height, VkImage &image,
                                        VkDeviceMemory &imageMemory) {
  int imageWidth = width;
  int imageHeight = height;

  VkDeviceSize imageSize = imageWidth * imageHeight * 4;

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;

  createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
               stagingBufferMemory);

  void *data;
  vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);

  memcpy(data, textureData, (size_t)imageSize);

  vkUnmapMemory(device, stagingBufferMemory);

  createImage(imageWidth, imageHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
              VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image,
              imageMemory);

  transitionImageLayout(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  copyBufferToImage(stagingBuffer, image, (uint32_t)imageWidth, (uint32_t)imageHeight);
  transitionImageLayout(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  vkDestroyBuffer(device, stagingBuffer, nullptr);
  vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void RendererVulkan::createTextureImageView(VkImage &image, VkImageView &imageView) {
  imageView = createImageView(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
}

void RendererVulkan::createTextureSampler(VkSampler &sampler) {
  VkPhysicalDeviceProperties properties{};
  vkGetPhysicalDeviceProperties(physicalDevice, &properties);

  VkSamplerCreateInfo samplerInfo{};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter = VK_FILTER_LINEAR;
  samplerInfo.minFilter = VK_FILTER_LINEAR;
  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.anisotropyEnable = VK_TRUE;
  samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
  samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;
  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  samplerInfo.mipLodBias = 0.0f;
  samplerInfo.minLod = 0.0f;
  samplerInfo.maxLod = 0.0f;

  if (vkCreateSampler(device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
    Util::Error("Failed to create texture sampler");
}

void RendererVulkan::createCommandBuffer() {
  commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

  // command buffer allocation
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = commandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

  if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
    Util::Error("Failed to create command buffer");
  }
}

void RendererVulkan::createSyncObjects() {
  imageAvailableSem.resize(MAX_FRAMES_IN_FLIGHT);
  renderFinishedSem.resize(MAX_FRAMES_IN_FLIGHT);
  inFlightFence.resize(MAX_FRAMES_IN_FLIGHT);

  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSem[i]) != VK_SUCCESS)
      Util::Error("Failed to create semaphore");

    if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSem[i]) != VK_SUCCESS)
      Util::Error("Failed to create semaphore");

    if (vkCreateFence(device, &fenceInfo, nullptr, &inFlightFence[i]) != VK_SUCCESS)
      Util::Error("Failed to create fence");
  }
}

// ====================================================================================================================
// Helper Methods
// ====================================================================================================================
bool RendererVulkan::hasStencilComponent(VkFormat format) {
  return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

VkFormat RendererVulkan::findDepthFormat() {
  return findSupportedFormat({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                             VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

VkFormat RendererVulkan::findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling,
                                             VkFormatFeatureFlags features) {
  for (VkFormat format : candidates) {
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);
    if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
      return format;
    else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
      return format;
  }

  Util::Error("");
  return {};
}

VkImageView RendererVulkan::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) {
  VkImageViewCreateInfo viewInfo{};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = image;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = format;
  viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
  viewInfo.subresourceRange.aspectMask = aspectFlags;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = 1;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;

  VkImageView imageView;
  if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
    Util::Error("Failed to create image view");

  return imageView;
}

void RendererVulkan::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
  VkCommandBuffer commandBuffer = beginSingleTimeCommands();

  VkBufferImageCopy region{};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;
  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = 1;
  region.imageOffset = {0, 0, 0};
  region.imageExtent = {width, height, 1};

  vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

  endSingleTimeCommands(commandBuffer);
}

void RendererVulkan::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout,
                                           VkImageLayout newLayout) {
  VkCommandBuffer commandBuffer = beginSingleTimeCommands();
  VkImageMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = oldLayout;
  barrier.newLayout = newLayout;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = image;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;

  if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    if (hasStencilComponent(format))
      barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
  } else {
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  }

  VkPipelineStageFlags sourceStage;
  VkPipelineStageFlags destinationStage;

  if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  } else {
    Util::Error("Unsupported layout transition");
  }

  vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

  endSingleTimeCommands(commandBuffer);
}

VkCommandBuffer RendererVulkan::beginSingleTimeCommands() {
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = commandPool;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(commandBuffer, &beginInfo);

  return commandBuffer;
}

void RendererVulkan::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(graphicsQueue);

  vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void RendererVulkan::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
                                 VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image,
                                 VkDeviceMemory &imageMemory) {
  VkImageCreateInfo imageInfo{};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.extent.width = width;
  imageInfo.extent.height = height;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;
  imageInfo.format = format;
  imageInfo.tiling = tiling;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.usage = usage;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.flags = 0;

  if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS)
    Util::Error("Failed to create texture image");

  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(device, image, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

  if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
    Util::Error("Failed to allocate texture image memory");

  vkBindImageMemory(device, image, imageMemory, 0);
}

void RendererVulkan::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                                  VkBuffer &buffer, VkDeviceMemory &memory) {
  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
    Util::Error("Failed to create vertex buffer");

  VkMemoryRequirements mem;
  vkGetBufferMemoryRequirements(device, buffer, &mem);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = mem.size;
  allocInfo.memoryTypeIndex = findMemoryType(mem.memoryTypeBits, properties);

  if (vkAllocateMemory(device, &allocInfo, nullptr, &memory) != VK_SUCCESS)
    Util::Error("Failed to allocate vertex buffer memory");

  vkBindBufferMemory(device, buffer, memory, 0);
}

void RendererVulkan::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
  VkCommandBuffer commandBuffer = beginSingleTimeCommands();

  VkBufferCopy copyRegion{};
  copyRegion.srcOffset = 0;
  copyRegion.dstOffset = 0;
  copyRegion.size = size;

  vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

  endSingleTimeCommands(commandBuffer);
}

VkPresentModeKHR RendererVulkan::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &available) {
  for (const auto &presentMode : available) {
    if (presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
      return presentMode;

    if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
      return presentMode;
  }
  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D RendererVulkan::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  } else {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    VkExtent2D actual = {(uint32_t)width, (uint32_t)height};

    actual.width = std::clamp(actual.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actual.height = std::clamp(actual.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    return actual;
  }
}

VkSurfaceFormatKHR RendererVulkan::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &available) {
  for (const auto &format : available) {
    if (format.format == VK_FORMAT_B8G8R8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return format;
    }
  }
  return available[0];
}

bool RendererVulkan::checkValidationLayerSupport() {
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

void RendererVulkan::createSurface() {
  if (glfwCreateWindowSurface(instance, window, nullptr, &surface))
    Util::Error("Failed to create window surface");

#ifdef PRINT_EXTENSIONS
  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
  std::vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
  std::cout << "Availible extensions:\n";
  for (auto &extension : extensions) {
    std::cout << "\t" << extension.extensionName << "\n";
  }
#endif
}

bool RendererVulkan::checkDeviceExtensionSupport(VkPhysicalDevice pDevice) {
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(pDevice, nullptr, &extensionCount, nullptr);
  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(pDevice, nullptr, &extensionCount, availableExtensions.data());
  bool found = false;
  for (const auto &need : deviceExtensions) {
    found = false;
    for (const auto &available : availableExtensions) {
      if (strcmp(need, available.extensionName) == 0) {
        found = true;
      }
    }
    if (!found)
      return false;
  }
  return true;
}

RendererVulkan::SwapchainSupportDetails RendererVulkan::querySwapchainSupport(VkPhysicalDevice pDevice) {
  SwapchainSupportDetails details;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pDevice, surface, &details.capabilities);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(pDevice, surface, &formatCount, nullptr);
  if (formatCount > 0) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(pDevice, surface, &formatCount, details.formats.data());
  }

  uint32_t presentCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(pDevice, surface, &presentCount, nullptr);
  if (presentCount > 0) {
    details.presentModes.resize(presentCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(pDevice, surface, &presentCount, details.presentModes.data());
  }

  return details;
}

bool RendererVulkan::isDeviceCompatible(VkPhysicalDevice pDevice) {
  QueueFamily queue = setupQueueFamilies(pDevice);

  bool extensionSupported = checkDeviceExtensionSupport(pDevice);
  bool swapchainSupported = false;
  if (extensionSupported) {
    SwapchainSupportDetails swapchainSupport = querySwapchainSupport(pDevice);
    swapchainSupported = !swapchainSupport.presentModes.empty() && !swapchainSupport.formats.empty();
  }

  VkPhysicalDeviceFeatures supportedFeatures;
  vkGetPhysicalDeviceFeatures(pDevice, &supportedFeatures);

  return queue.compatible() && extensionSupported && swapchainSupported && supportedFeatures.samplerAnisotropy;
}

RendererVulkan::QueueFamily RendererVulkan::setupQueueFamilies(VkPhysicalDevice pDevice) {
  uint32_t familyCount;
  vkGetPhysicalDeviceQueueFamilyProperties(pDevice, &familyCount, nullptr);

  std::vector<VkQueueFamilyProperties> families(familyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(pDevice, &familyCount, families.data());

  QueueFamily queue;
  uint32_t i = 0;
  VkBool32 presentSupport = false;
  for (const auto &family : families) {
    if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
      queue.graphics = i;

    presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(pDevice, i, surface, &presentSupport);
    if (presentSupport)
      queue.present = i;

    i++;
  }
  return queue;
}

void RendererVulkan::createShaders(std::string vertexFilePath, std::string fragmentFilePath,
                                   std::string tesselationFilePath, std::string geometeryFilePath) {

  VkShaderModule vertex, fragment, tesselation, geometery;

  std::vector<char> vertexShader = Util::readFile(vertexFilePath);
  vertex = createShaderModule(vertexShader);
  shaders.vertex = vertex;

  if (!tesselationFilePath.empty()) {
    std::vector<char> tessShader = Util::readFile(tesselationFilePath);
    tesselation = createShaderModule(tessShader);
    shaders.tesselation = tesselation;
  }

  std::vector<char> fragmentShader = Util::readFile(fragmentFilePath);
  fragment = createShaderModule(fragmentShader);
  shaders.fragment = fragment;

  if (!geometeryFilePath.empty()) {
    std::vector<char> geomShader = Util::readFile(geometeryFilePath);
    geometery = createShaderModule(geomShader);
    shaders.geometery = geometery;
  }
}

VkShaderModule RendererVulkan::createShaderModule(std::vector<char> &shader) {

  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = shader.size();
  createInfo.pCode = reinterpret_cast<const uint32_t *>(shader.data());

  VkShaderModule module;

  if (vkCreateShaderModule(device, &createInfo, nullptr, &module) != VK_SUCCESS)
    Util::Error("Failed to create shader module");

  return module;
}

void RendererVulkan::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = 0;
  beginInfo.pInheritanceInfo = nullptr;

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    Util::Error("Failed to begin recording command buffer");

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = colorAndDepthRenderPass;
  renderPassInfo.framebuffer = framebuffers[imageIndex];
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = swapchainExtent;

  std::array<VkClearValue, 2> clearValues{};
  clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
  clearValues[1].depthStencil = {1.0f, 0};
  renderPassInfo.clearValueCount = (uint32_t)clearValues.size();
  renderPassInfo.pClearValues = clearValues.data();

  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, blinn.pipeline);

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)swapchainExtent.width;
  viewport.height = (float)swapchainExtent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = swapchainExtent;
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

  // assuming, we are combining all objects into one huge mesh/index buffer
  VkBuffer buffers[] = {meshBuffer};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
  vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

  for (size_t i = 0; i < OBJECT_COUNT; i++) {
    uint32_t dOffset = (uint32_t)i * (uint32_t)alignment;
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, blinn.pipelineLayout, 0, 1,
                            &blinn.descriptorSets[currentFrame], 1, &dOffset);

    vkCmdDrawIndexed(commandBuffer, indexCount[i], 1, indexOffsets[i], vertexOffsets[i], 0);
  }

  vkCmdEndRenderPass(commandBuffer);

  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    Util::Error("Failed to record command buffer");
}

void RendererVulkan::drawScene() {
  vkWaitForFences(device, 1, &inFlightFence[currentFrame], VK_TRUE, UINT64_MAX);

  uint32_t imageIndex;
  VkResult result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvailableSem[currentFrame],
                                          VK_NULL_HANDLE, &imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreateSwapchain();
    return;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    Util::Error("Failed to acquire swap chain image");

  vkResetFences(device, 1, &inFlightFence[currentFrame]);

  vkResetCommandBuffer(commandBuffers[currentFrame], 0);

  recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

  updateUniformBuffer(currentFrame);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {imageAvailableSem[currentFrame]};
  VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

  VkSemaphore signalSemaphores[] = {renderFinishedSem[currentFrame]};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence[currentFrame]) != VK_SUCCESS)
    Util::Error("Failed to submit draw command buffer");

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapchains[] = {swapchain};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapchains;
  presentInfo.pImageIndices = &imageIndex;
  presentInfo.pResults = nullptr;

  result = vkQueuePresentKHR(presentQueue, &presentInfo);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || resized) {
    resized = false;
    recreateSwapchain();
  } else if (result != VK_SUCCESS)
    Util::Error("Failed to present swap chain image");

  currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void RendererVulkan::updateUniformBuffer(uint32_t frame) {
  Math::Matrix4 proj = perspectiveMatrix(60, (f32)WIDTH / HEIGHT);
  Math::Matrix4 view = scene->camera.viewMatrix();
  for (size_t i = 0; i < OBJECT_COUNT; i++) {
    blinn.ubos[i].view = view;
    blinn.ubos[i].proj = proj;
    blinn.ubos[i].model = scene->objects[i].getModelMatrix();

    blinn.ubos[i].mvn = (view * blinn.ubos[i].model).toMatrix3x3();
    blinn.ubos[i].mvn.inverse();
    blinn.ubos[i].mvn.transpose();
  }

  memcpy(blinn.uniformBuffersMapped[frame], blinn.ubos.data(), dynamicUniformBufferSize);

  VkMappedMemoryRange memoryRange{};
  memoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
  memoryRange.memory = blinn.uniformBuffersMemory[frame];
  memoryRange.size = dynamicUniformBufferSize;
  memoryRange.offset = 0;
  memoryRange.pNext = nullptr;

  vkFlushMappedMemoryRanges(device, 1, &memoryRange);
}

void RendererVulkan::cleanSwapchain() {
  vkDestroyImageView(device, depthImageView, nullptr);
  vkDestroyImage(device, depthImage, nullptr);
  vkFreeMemory(device, depthImageMemory, nullptr);

  for (auto framebuffer : framebuffers)
    vkDestroyFramebuffer(device, framebuffer, nullptr);

  for (auto imageView : swapchainImageViews)
    vkDestroyImageView(device, imageView, nullptr);

  vkDestroySwapchainKHR(device, swapchain, nullptr);
}

void RendererVulkan::recreateSwapchain() {
  int width = 0, height = 0;
  while (width == 0 || height == 0) {
    glfwGetFramebufferSize(window, &width, &height);
    glfwWaitEvents();
  }

  WIDTH = width;
  HEIGHT = height;
  r = (f32)WIDTH / 2.0;
  l = -r;

  t = (f32)HEIGHT / 2.0;
  b = -t;

  vkDeviceWaitIdle(device);

  cleanSwapchain();

  createSwapchain();
  createImageViews();
  createDepthResources();
  createFrameBuffers();
}

void RendererVulkan::resize() { resized = true; }

VkDeviceSize RendererVulkan::getMinUniformBufferOffsetAlignment() {
  VkPhysicalDeviceProperties prop;
  vkGetPhysicalDeviceProperties(physicalDevice, &prop);
  return prop.limits.minUniformBufferOffsetAlignment;
}

VkDeviceSize RendererVulkan::getUniformBufferAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
  if (minOffsetAlignment > 0)
    return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);

  return instanceSize;
}

uint32_t RendererVulkan::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties mem;
  vkGetPhysicalDeviceMemoryProperties(physicalDevice, &mem);

  for (uint32_t i = 0; i < mem.memoryTypeCount; i++)
    if (typeFilter & (1 << i) && (mem.memoryTypes[i].propertyFlags & properties) == properties)
      return i;

  Util::Error("Failed to find suitable memory type");
  return UINT32_MAX;
}

Math::Matrix4 RendererVulkan::perspectiveMatrix(f32 fov, f32 aspect) {
  f32 fovy = TO_RADIANS(fov);
  f32 focalLength = (f32)1.0 / std::tan(fovy * 0.5);
  f32 A = f / (-f + n);
  f32 B = (f * n) / (-f + n);
  return Math::Matrix4(focalLength / aspect, 0, 0, 0, //
                       0, -focalLength, 0, 0,         //
                       0, 0, A, B,                    //
                       0, 0, -1, 0);
}

} // namespace Renderer
