#include "renderer.hpp"
#include <GLFW/glfw3.h>

namespace Renderer {

Renderer::Renderer(int w, int h) {
  WIDTH = w;
  HEIGHT = h;

  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  this->window =
      glfwCreateWindow(WIDTH, HEIGHT, "Beast Gohan", nullptr, nullptr);

  glfwSetWindowUserPointer(window, this);
  glfwSetKeyCallback(window, keyCallback);
  glfwSetFramebufferSizeCallback(window, resizeCallback);

  rendererbackend.init(this->window);
}
void Renderer::draw() { rendererbackend.drawFrame(); }
void Renderer::poll() { glfwPollEvents(); }

bool Renderer::running() { return !glfwWindowShouldClose(window); }

void Renderer::resize() { rendererbackend.resize(); }

void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods) {
  if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

void resizeCallback(GLFWwindow *window, int width, int height) {
  auto app = reinterpret_cast<Renderer *>(glfwGetWindowUserPointer(window));
  app->resize();
}

} // namespace Renderer
