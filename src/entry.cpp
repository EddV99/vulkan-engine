/**
 * @brief
 */
#include "renderer/renderer-vulkan.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

int main(void) {
  Renderer::RendererVulkan renderer;
  renderer.init();

  while (true) {
    renderer.draw();
  }
}
