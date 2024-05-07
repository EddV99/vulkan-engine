#include <cstring>
#include <limits>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

static std::vector<char> readFile(const std::string &filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("failed to open file");
  }

  size_t fileSize = (size_t)file.tellg();
  std::vector<char> bytes(fileSize);

  file.seekg(0);
  file.read(bytes.data(), fileSize);

  return bytes;
}

class Application {
public:
  void run() {
    init();
    mainloop();
    clean();
  }

private:
  struct QueueFamily {
    std::optional<uint32_t> graphics;
    std::optional<uint32_t> present;

    bool compatible() { return graphics.has_value() && present.has_value(); }
  };
  struct SwapchainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
  };

  SwapchainSupportDetails
  querySwapchainSupport(VkPhysicalDevice physicalDevice) {
    SwapchainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface,
                                              &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount,
                                         nullptr);
    if (formatCount > 0) {
      details.formats.resize(formatCount);
      vkGetPhysicalDeviceSurfaceFormatsKHR(
          physicalDevice, surface, &formatCount, details.formats.data());
    }

    uint32_t presentCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface,
                                              &presentCount, nullptr);
    if (presentCount > 0) {
      details.presentModes.resize(presentCount);
      vkGetPhysicalDeviceSurfacePresentModesKHR(
          physicalDevice, surface, &presentCount, details.presentModes.data());
    }

    return details;
  }
  void init() {
    initGLFW();
    initVulkan();
  }
  void initGLFW() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Edd Vulkan", nullptr, nullptr);
  }
  void initVulkan() {
    // check validation layer support
    // ---------------------------------------------------------------------------------------------------------------
    if (enableValidationLayers && !checkValidationLayerSupport()) {
      throw std::runtime_error(
          "Validation layers requested, but failed to get");
    }

    // instance setup
    // ---------------------------------------------------------------------------------------------------------------
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "edd";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    instanceCreateInfo.enabledExtensionCount = glfwExtensionCount;
    instanceCreateInfo.ppEnabledExtensionNames = glfwExtensions;

    // setup validation layers if wanted
    // ---------------------------------------------------------------------------------------------------------------
    if (enableValidationLayers) {
      instanceCreateInfo.enabledLayerCount = (uint32_t)validationLayers.size();
      instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
      instanceCreateInfo.enabledLayerCount = 0;
    }

    if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) !=
        VK_SUCCESS) {
      throw std::runtime_error("Failed to create instance");
    }

    // setup surface
    // ---------------------------------------------------------------------------------------------------------------
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface)) {
      throw std::runtime_error("Failed to create window surface");
    }

    // print out available extensions
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                           extensions.data());
    std::cout << "Availible extensions:\n";
    for (auto &extension : extensions) {
      std::cout << "\t" << extension.extensionName << "\n";
    }

    // pick physical device (gpu)
    // ---------------------------------------------------------------------------------------------------------------
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount <= 0) {
      throw std::runtime_error("No GPU with vulkan support");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for (const auto &d : devices) {
      if (isDeviceCompatible(d)) {
        phyDevice = d;
        break;
      }
    }

    if (phyDevice == VK_NULL_HANDLE) {
      throw std::runtime_error("No GPU is suitable");
    }

    // setup logical device
    // ---------------------------------------------------------------------------------------------------------------
    QueueFamily queueFamily = findQueueFamilies(phyDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> unique = {queueFamily.graphics.value(),
                                 queueFamily.present.value()};
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

    if (vkCreateDevice(phyDevice, &createInfo, nullptr, &device) !=
        VK_SUCCESS) {
      throw std::runtime_error("Failed to create logical device");
    }

    // setup swapchain
    // ---------------------------------------------------------------------------------------------------------------
    SwapchainSupportDetails swapchainSupport = querySwapchainSupport(phyDevice);
    VkSurfaceFormatKHR surfaceFormat =
        chooseSwapSurfaceFormat(swapchainSupport.formats);
    VkPresentModeKHR presentMode =
        chooseSwapPresentMode(swapchainSupport.presentModes);

    VkExtent2D extent = chooseSwapExtent(swapchainSupport.capabilities);

    uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;

    if (swapchainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapchainSupport.capabilities.maxImageCount) {
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

    uint32_t indices[] = {queueFamily.graphics.value(),
                          queueFamily.present.value()};

    if (queueFamily.graphics != queueFamily.present) {
      swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
      swapchainCreateInfo.queueFamilyIndexCount = 2;
      swapchainCreateInfo.pQueueFamilyIndices = indices;
    } else {
      swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
      swapchainCreateInfo.queueFamilyIndexCount = 0;
      swapchainCreateInfo.pQueueFamilyIndices = nullptr;
    }
    swapchainCreateInfo.preTransform =
        swapchainSupport.capabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = presentMode;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr,
                             &swapchain) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create swapchain");
    }

    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
    swapchainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapchain, &imageCount,
                            swapchainImages.data());
    swapchainImageFormat = surfaceFormat.format;
    swapchainExtent = extent;

    // create image views
    // ---------------------------------------------------------------------------------------------------------------
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

      viewImageCreateInfo.subresourceRange.aspectMask =
          VK_IMAGE_ASPECT_COLOR_BIT;
      viewImageCreateInfo.subresourceRange.baseMipLevel = 0;
      viewImageCreateInfo.subresourceRange.levelCount = 1;
      viewImageCreateInfo.subresourceRange.baseArrayLayer = 0;
      viewImageCreateInfo.subresourceRange.layerCount = 1;

      if (vkCreateImageView(device, &viewImageCreateInfo, nullptr,
                            &swapchainImageViews[i]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create image view");
      }
    }

    // setup graphics pipeline
    // ---------------------------------------------------------------------------------------------------------------
    auto vertexShader = readFile("../src/shaders/vert.spv");
    auto fragmentShader = readFile("../src/shaders/frag.spv");

    VkShaderModule vertexShaderModule = createShaderModule(vertexShader);
    VkShaderModule fragmentShaderModule = createShaderModule(fragmentShader);

    VkPipelineShaderStageCreateInfo vertexShaderStageInfo{};
    vertexShaderStageInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShaderStageInfo.module = vertexShaderModule;
    vertexShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{};
    fragmentShaderStageInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentShaderStageInfo.module = fragmentShaderModule;
    fragmentShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertexShaderStageInfo,
                                                      fragmentShaderStageInfo};

    // dynamic part of pipeline
    std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT,
                                                 VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = (uint32_t)dynamicStates.size();
    dynamicState.pDynamicStates = dynamicStates.data();

    // vertex input (not used for now)
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr;

    // input assembly
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType =
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
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
    rasterizer.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
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
    multisampling.sType =
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
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
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor =
        VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor =
        VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType =
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
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

    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr,
                               &pipelineLayout) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create pipeline layout");
    }

    // destroy modules after setup
    vkDestroyShaderModule(device, vertexShaderModule, nullptr);
    vkDestroyShaderModule(device, fragmentShaderModule, nullptr);

    // get graphics queue handle
    // ---------------------------------------------------------------------------------------------------------------
    vkGetDeviceQueue(device, queueFamily.graphics.value(), 0, &graphicsQueue);
    // get present queue handle
    vkGetDeviceQueue(device, queueFamily.present.value(), 0, &presentQueue);
  }

  VkShaderModule createShaderModule(const std::vector<char> &source) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = source.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(source.data());
    VkShaderModule module;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &module) !=
        VK_SUCCESS) {
      throw std::runtime_error("Failed to create shader module");
    }

    return module;
  }

  QueueFamily findQueueFamilies(VkPhysicalDevice physicalDevice) {
    uint32_t familyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount,
                                             nullptr);

    std::vector<VkQueueFamilyProperties> families(familyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount,
                                             families.data());

    QueueFamily family;
    uint32_t i = 0;
    VkBool32 presentSupport = false;
    for (const auto &fam : families) {
      if (fam.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        family.graphics = i;
      }

      presentSupport = false;
      vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface,
                                           &presentSupport);
      if (presentSupport) {
        family.present = i;
      }

      i++;
    }
    return family;
  }
  void mainloop() {
    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();
    }
  }

  void clean() {
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);

    for (auto imageView : swapchainImageViews) {
      vkDestroyImageView(device, imageView, nullptr);
    }
    vkDestroySwapchainKHR(device, swapchain, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(window);
    glfwTerminate();
  }
  bool isDeviceCompatible(VkPhysicalDevice physicalDevice) {
    /* VkPhysicalDeviceProperties properties; */
    /* vkGetPhysicalDeviceProperties(physicalDevice, &properties); */

    /* VkPhysicalDeviceFeatures features; */
    /* vkGetPhysicalDeviceFeatures(physicalDevice, &features); */
    QueueFamily queue = findQueueFamilies(physicalDevice);

    bool extensionSupported = checkDeviceExtensionSupport(physicalDevice);
    bool swapchainSupported = false;
    if (extensionSupported) {
      SwapchainSupportDetails swapchainSupport =
          querySwapchainSupport(physicalDevice);
      swapchainSupported = !swapchainSupport.presentModes.empty() &&
                           !swapchainSupport.formats.empty();
    }
    return queue.compatible() && extensionSupported && swapchainSupported;
  }

  VkSurfaceFormatKHR
  chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &available) {
    for (const auto &format : available) {
      if (format.format == VK_FORMAT_B8G8R8_SRGB &&
          format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
        return format;
      }
    }
    return available[0];
  }
  VkPresentModeKHR
  chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &available) {
    for (const auto &presentMode : available) {
      if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
        return presentMode;
      }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
  }

  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
    if (capabilities.currentExtent.width !=
        std::numeric_limits<uint32_t>::max()) {
      return capabilities.currentExtent;
    } else {
      int width, height;
      glfwGetFramebufferSize(window, &width, &height);
      VkExtent2D actual = {(uint32_t)width, (uint32_t)height};

      actual.width = std::clamp(actual.width, capabilities.minImageExtent.width,
                                capabilities.maxImageExtent.width);
      actual.height =
          std::clamp(actual.height, capabilities.minImageExtent.height,
                     capabilities.maxImageExtent.height);
      return actual;
    }
  }
  bool checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr,
                                         &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(
        physicalDevice, nullptr, &extensionCount, availableExtensions.data());
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
  bool checkValidationLayerSupport() {
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

  GLFWwindow *window;
  const int WIDTH = 800;
  const int HEIGHT = 600;

  VkInstance instance;
  VkPhysicalDevice phyDevice = VK_NULL_HANDLE;
  VkDevice device;
  VkQueue graphicsQueue;
  VkQueue presentQueue;
  VkSurfaceKHR surface;
  VkSwapchainKHR swapchain;
  std::vector<VkImage> swapchainImages;
  std::vector<VkImageView> swapchainImageViews;
  VkFormat swapchainImageFormat;
  VkExtent2D swapchainExtent;
  VkPipelineLayout pipelineLayout;

  const std::vector<const char *> validationLayers = {
      "VK_LAYER_KHRONOS_validation"};

  const std::vector<const char *> deviceExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#ifdef NDEBUG
  const bool enableValidationLayers = false;
#else
  const bool enableValidationLayers = true;
#endif
};

int main() {
  Application app;

  try {
    app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << "\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
