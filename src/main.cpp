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


    // get graphics queue handle
    // ---------------------------------------------------------------------------------------------------------------
    vkGetDeviceQueue(device, queueFamily.graphics.value(), 0, &graphicsQueue);
    // get present queue handle
    vkGetDeviceQueue(device, queueFamily.present.value(), 0, &presentQueue);
  }

  VkShaderModule createShaderModule(const std::vector<char>& source){

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
