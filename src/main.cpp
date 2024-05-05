#include <cstring>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <optional>
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
  };
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
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily.graphics.value();
    queueCreateInfo.queueCount = 1;

    float priority = 1.0f;
    queueCreateInfo.pQueuePriorities = &priority;
    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures = &deviceFeatures;

    // for back compatibility
    createInfo.enabledExtensionCount = 0;
    if (enableValidationLayers) {
      createInfo.enabledLayerCount = (uint32_t)validationLayers.size();
      createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
      createInfo.enabledLayerCount = 0;
    }

    if(vkCreateDevice(phyDevice, &createInfo, nullptr, &device) != VK_SUCCESS){
      throw std::runtime_error("Failed to create logical device");
    }

    // get graphics queue handle
    vkGetDeviceQueue(device, queueFamily.graphics.value(), 0, &graphicsQueue);
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
    for (const auto &fam : families) {
      if (fam.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        family.graphics = i;
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
    return queue.graphics.has_value();
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

  const std::vector<const char *> validationLayers = {
      "VK_LAYER_KHRONOS_validation"};

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
