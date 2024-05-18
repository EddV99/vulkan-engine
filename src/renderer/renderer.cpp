#include "renderer.hpp"
#include <GLFW/glfw3.h>

#include "renderer-vulkan.hpp"

namespace Renderer {

Renderer::Renderer(int w, int h) {
  WIDTH = w;
  HEIGHT = h;

  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  this->window =
      glfwCreateWindow(WIDTH, HEIGHT, "Beast Gohan", nullptr, nullptr);

  glfwSetKeyCallback(window, keyCallback);

  rendererbackend.init(this->window);
}
void Renderer::draw() { rendererbackend.drawFrame(); }
void Renderer::poll() { glfwPollEvents(); }

bool Renderer::running() { return !glfwWindowShouldClose(window); }

void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods) {
  if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

} // namespace Renderer
