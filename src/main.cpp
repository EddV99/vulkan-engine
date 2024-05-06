#include <cstring>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

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
    if (enableValidationLayers && !checkValidationLayerSupport()) {
      throw std::runtime_error(
          "Validation layers requested, but failed to get");
    }

    // instance setup
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

    // get graphics queue handle
    vkGetDeviceQueue(device, queueFamily.graphics.value(), 0, &graphicsQueue);
    // get present queue handle
    vkGetDeviceQueue(device, queueFamily.present.value(), 0, &presentQueue);
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
        SwapchainSupportDetails swapchainSupport = querySwapchainSupport(physicalDevice);
        swapchainSupported = !swapchainSupport.presentModes.empty() && !swapchainSupport.formats.empty();
    }
    return queue.compatible() && extensionSupported && swapchainSupported;
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
