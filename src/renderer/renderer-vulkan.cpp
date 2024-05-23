/**
 * @file renderer.cpp
 */
#include "renderer-vulkan.hpp"
#include "../util/util.hpp"

#include <GLFW/glfw3.h>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>
#include <vulkan/vulkan_core.h>

namespace Renderer {

RendererVulkan::RendererVulkan(uint32_t width, uint32_t height) {
  WIDTH = width;
  HEIGHT = height;
}

RendererVulkan::~RendererVulkan() {
  vkDeviceWaitIdle(device);

  cleanSwapchain();

  for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(device, imageAvailableSem[i], nullptr);
    vkDestroySemaphore(device, renderFinishedSem[i], nullptr);
    vkDestroyFence(device, inFlightFence[i], nullptr);
  }

  vkDestroyCommandPool(device, commandPool, nullptr);

  vkDestroyPipeline(device, graphicsPipeline, nullptr);
  vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
  vkDestroyRenderPass(device, renderPass, nullptr);

  vkDestroyDevice(device, nullptr);
  vkDestroySurfaceKHR(instance, surface, nullptr);
  vkDestroyInstance(instance, nullptr);

  glfwDestroyWindow(window);
  glfwTerminate();
}

// ====================================================================================================================
//     Initialization
// ====================================================================================================================
void RendererVulkan::init(GLFWwindow *window) {
  if (enableValidationLayers && !checkValidationLayerSupport())
    Util::Error("Validation layers requested, but failed to get");

  this->window = window;
  createInstance();
  createSurface();
  pickPhysicalDevice();
  createDevice();
  createSwapchain();
  createImageViews();
  createRenderPass();
  createGraphicsPipeline();
  createFrameBuffers();
  createCommandBuffer();
  createSyncObjects();

  // get graphics queue handle
  vkGetDeviceQueue(device, queueFamily.graphics.value(), 0, &graphicsQueue);
  // get present queue handle
  vkGetDeviceQueue(device, queueFamily.present.value(), 0, &presentQueue);
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

  if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
    Util::Error("Failed to create instance");
  }
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
}

void RendererVulkan::createSwapchain() {
  SwapchainSupportDetails swapchainSupport = querySwapchainSupport(physicalDevice);
  VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapchainSupport.formats);
  VkPresentModeKHR presentMode = chooseSwapPresentMode(swapchainSupport.presentModes);

  extent = chooseSwapExtent(swapchainSupport.capabilities);

  uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;

  if (swapchainSupport.capabilities.maxImageCount > 0 && imageCount > swapchainSupport.capabilities.maxImageCount) {
    imageCount = swapchainSupport.capabilities.maxImageCount;
  }

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

  if (vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain) != VK_SUCCESS) {
    Util::Error("Failed to create swapchain");
  }

  vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
  swapchainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages.data());
  swapchainImageFormat = surfaceFormat.format;
  swapchainExtent = extent;
}

void RendererVulkan::createImageViews() {
  swapchainImageViews.resize(swapchainImages.size());
  for (int i = 0; i < swapchainImages.size(); i++) {
    VkImageViewCreateInfo viewImageCreateInfo{};
    viewImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewImageCreateInfo.image = swapchainImages[i];
    viewImageCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewImageCreateInfo.format = swapchainImageFormat;
    viewImageCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewImageCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewImageCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewImageCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    viewImageCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewImageCreateInfo.subresourceRange.baseMipLevel = 0;
    viewImageCreateInfo.subresourceRange.levelCount = 1;
    viewImageCreateInfo.subresourceRange.baseArrayLayer = 0;
    viewImageCreateInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device, &viewImageCreateInfo, nullptr, &swapchainImageViews[i]) != VK_SUCCESS) {
      Util::Error("Failed to create image view");
    }
  }
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

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;

  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkRenderPassCreateInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = &colorAttachment;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
    Util::Error("Failed to create render pass");
  }
}

void RendererVulkan::createGraphicsPipeline() {
  auto vertexShader = Util::readFile("../src/shaders/vert.spv");
  auto fragmentShader = Util::readFile("../src/shaders/frag.spv");

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
  uint32_t count = vertexBuffers.bindingDescriptions.size();
  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = count;
  vertexInputInfo.pVertexBindingDescriptions = vertexBuffers.bindingDescriptions.data();
  vertexInputInfo.vertexAttributeDescriptionCount = count;
  if (count == 1)
    vertexInputInfo.pVertexAttributeDescriptions = vertexBuffers.attributeOne.data();
  else if (count == 2)
    vertexInputInfo.pVertexAttributeDescriptions = vertexBuffers.attributeTwo.data();
  else if (count == 3)
    vertexInputInfo.pVertexAttributeDescriptions = vertexBuffers.attributeThree.data();

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
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
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

  // depth/stencil testing (not using)
  // VkPipelineDepthStencilStateCreateInfo depthStencilInfo;

  // color blending setup
  VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  colorBlendAttachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_FALSE;
  /* colorBlendAttachment.srcColorBlendFactor = */
  /*     VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; */
  /* colorBlendAttachment.dstColorBlendFactor = */
  /*     VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; */
  /* colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; */
  /* colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; */
  /* colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; */
  /* colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; */

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
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pSetLayouts = nullptr;
  pipelineLayoutInfo.pushConstantRangeCount = 0;
  pipelineLayoutInfo.pPushConstantRanges = nullptr;

  if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
    Util::Error("Failed to create pipeline layout");
  }

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
  pipelineInfo.pDepthStencilState = nullptr;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = &dynamicState;
  pipelineInfo.layout = pipelineLayout;
  pipelineInfo.renderPass = renderPass;
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
  pipelineInfo.basePipelineIndex = -1;

  if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
    Util::Error("Failed to create graphics pipeline");
  }
  // destroy modules after setup
  vkDestroyShaderModule(device, vertexShaderModule, nullptr);
  vkDestroyShaderModule(device, fragmentShaderModule, nullptr);
}

void RendererVulkan::createFrameBuffers() {

  int i = 0;
  framebuffers.resize(swapchainImageViews.size());
  for (auto &imageView : swapchainImageViews) {
    VkImageView attachments[] = {imageView};

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS) {
      Util::Error("Failed to create framebuffer");
    }
    i++;
  }
}

void RendererVulkan::createCommandBuffer() {
  commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = queueFamily.graphics.value();

  if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
    Util::Error("Failed to create command pool");
  }

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
    if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSem[i]) != VK_SUCCESS) {
      Util::Error("Failed to create semaphore");
    }
    if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSem[i]) != VK_SUCCESS) {
      Util::Error("Failed to create semaphore");
    }
    if (vkCreateFence(device, &fenceInfo, nullptr, &inFlightFence[i]) != VK_SUCCESS) {
      Util::Error("Failed to create fence");
    }
  }
}

// ====================================================================================================================
// Helper Methods
// ====================================================================================================================
VkPresentModeKHR RendererVulkan::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &available) {
  for (const auto &presentMode : available) {
    if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return presentMode;
    }
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
  if (glfwCreateWindowSurface(instance, window, nullptr, &surface)) {
    Util::Error("Failed to create window surface");
  }

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
  return queue.compatible() && extensionSupported && swapchainSupported;
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
    if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      queue.graphics = i;
    }

    presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(pDevice, i, surface, &presentSupport);
    if (presentSupport) {
      queue.present = i;
    }

    i++;
  }
  return queue;
}

void RendererVulkan::createShaders(std::string vertexFilePath, std::string fragmentFilePath,
                                   std::string tesselationFilePath, std::string geometeryFilePath) {

  auto vertexShader = Util::readFile(vertexFilePath);
  auto fragmentShader = Util::readFile(fragmentFilePath);

  VkShaderModule vertex, fragment;
  vertex = createShaderModule(vertexShader);
  fragment = createShaderModule(fragmentShader);

  shaders.vertex = vertex;
  shaders.fragment = fragment;
}

VkShaderModule RendererVulkan::createShaderModule(std::vector<char> &shader) {

  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = shader.size();
  createInfo.pCode = reinterpret_cast<const uint32_t *>(shader.data());

  VkShaderModule module;

  if (vkCreateShaderModule(device, &createInfo, nullptr, &module) != VK_SUCCESS) {
    Util::Error("Failed to create shader module");
  }

  return module;
}

void RendererVulkan::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = 0;
  beginInfo.pInheritanceInfo = nullptr;

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
    Util::Error("Failed to begin recording command buffer");
  }

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = renderPass;
  renderPassInfo.framebuffer = framebuffers[imageIndex];
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = swapchainExtent;

  VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
  renderPassInfo.clearValueCount = 1;
  renderPassInfo.pClearValues = &clearColor;

  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

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

  vkCmdDraw(commandBuffer, 3, 1, 0, 0);

  vkCmdEndRenderPass(commandBuffer);

  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    Util::Error("Failed to record command buffer");
}

void RendererVulkan::createVertexBuffer(Mesh::Mesh mesh) {
  VkVertexInputBindingDescription vBindingDesc = Mesh::Mesh::getBindingDescriptionVertex();
  vertexBuffers.bindingDescriptions.push_back(vBindingDesc);

  if (mesh.hasNormals()) {
    VkVertexInputBindingDescription nBindingDesc = Mesh::Mesh::getBindingDescriptionNormal();
    vertexBuffers.bindingDescriptions.push_back(nBindingDesc);
  }

  if (mesh.hasUV()) {
    VkVertexInputBindingDescription uBindingDesc = Mesh::Mesh::getBindingDescriptionUV();
    vertexBuffers.bindingDescriptions.push_back(uBindingDesc);
  }

  if (!mesh.hasNormals() && !mesh.hasUV()) {
    std::array<VkVertexInputAttributeDescription, 1> AttribDesc{};

    auto vAttribDesc = Mesh::Mesh::getAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT);

    AttribDesc[0].binding = vAttribDesc.binding;
    AttribDesc[0].offset = vAttribDesc.offset;
    AttribDesc[0].format = vAttribDesc.format;
    AttribDesc[0].location = vAttribDesc.location;

    vertexBuffers.attributeOne = AttribDesc;
  }

  if (mesh.hasNormals() && !mesh.hasUV()) {
    std::array<VkVertexInputAttributeDescription, 2> AttribDesc{};

    auto vAttribDesc = Mesh::Mesh::getAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT);
    auto nAttribDesc = Mesh::Mesh::getAttributeDescription(1, 1, VK_FORMAT_R32G32B32_SFLOAT);

    AttribDesc[0].binding = vAttribDesc.binding;
    AttribDesc[0].offset = vAttribDesc.offset;
    AttribDesc[0].format = vAttribDesc.format;
    AttribDesc[0].location = vAttribDesc.location;

    AttribDesc[1].binding = nAttribDesc.binding;
    AttribDesc[1].offset = nAttribDesc.offset;
    AttribDesc[1].format = nAttribDesc.format;
    AttribDesc[1].location = nAttribDesc.location;

    vertexBuffers.attributeTwo = AttribDesc;
  }

  if (mesh.hasNormals() && mesh.hasUV()) {
    std::array<VkVertexInputAttributeDescription, 3> AttribDesc{};

    auto vAttribDesc = Mesh::Mesh::getAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT);
    auto nAttribDesc = Mesh::Mesh::getAttributeDescription(1, 1, VK_FORMAT_R32G32B32_SFLOAT);
    auto uAttribDesc = Mesh::Mesh::getAttributeDescription(2, 2, VK_FORMAT_R32G32_SFLOAT);

    AttribDesc[0].binding = vAttribDesc.binding;
    AttribDesc[0].offset = vAttribDesc.offset;
    AttribDesc[0].format = vAttribDesc.format;
    AttribDesc[0].location = vAttribDesc.location;

    AttribDesc[1].binding = nAttribDesc.binding;
    AttribDesc[1].offset = nAttribDesc.offset;
    AttribDesc[1].format = nAttribDesc.format;
    AttribDesc[1].location = nAttribDesc.location;

    AttribDesc[2].binding = uAttribDesc.binding;
    AttribDesc[2].offset = uAttribDesc.offset;
    AttribDesc[2].format = uAttribDesc.format;
    AttribDesc[2].location = uAttribDesc.location;

    vertexBuffers.attributeThree = AttribDesc;
  }
}

void RendererVulkan::createVertexBuffer(std::vector<Mesh::Mesh> meshes) {}

void RendererVulkan::createTexture() {}

void RendererVulkan::createUniformBuffer() {}

void RendererVulkan::drawFrame(FrameData frame) {}

void RendererVulkan::drawFrame() {
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

  if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence[currentFrame]) != VK_SUCCESS) {
    Util::Error("Failed to submit draw command buffer");
  }

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

void RendererVulkan::cleanSwapchain() {
  for (auto framebuffer : framebuffers)
    vkDestroyFramebuffer(device, framebuffer, nullptr);

  for (auto imageView : swapchainImageViews)
    vkDestroyImageView(device, imageView, nullptr);

  vkDestroySwapchainKHR(device, swapchain, nullptr);
}

void RendererVulkan::recreateSwapchain() {
  int width = 0, height = 0;
  glfwGetFramebufferSize(window, &width, &height);

  while (width == 0 || height == 0) {
    glfwGetFramebufferSize(window, &width, &height);
    glfwWaitEvents();
  }

  vkDeviceWaitIdle(device);

  cleanSwapchain();

  createSwapchain();
  createImageViews();
  createFrameBuffers();
}

void RendererVulkan::resize() { resized = true; }

} // namespace Renderer
