#include "renderer.hpp"
#include "renderer-core.hpp"
#include <GLFW/glfw3.h>

namespace Renderer {

Renderer::Renderer(int w, int h, Mesh::Scene scene) {
  WIDTH = w;
  HEIGHT = h;

  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  // glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  this->window = glfwCreateWindow(WIDTH, HEIGHT, "Beast Gohan", nullptr, nullptr);

  glfwSetWindowUserPointer(window, this);
  glfwSetKeyCallback(window, keyCallback);
  glfwSetFramebufferSizeCallback(window, resizeCallback);

  rendererbackend.init(this->window, scene);
}
void Renderer::draw() { rendererbackend.drawFrame(); }
void Renderer::draw(FrameData frame) { rendererbackend.drawFrame(frame); }
void Renderer::poll() { glfwPollEvents(); }

bool Renderer::running() { return !glfwWindowShouldClose(window); }

void Renderer::resize() { rendererbackend.resize(); }

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  (void)scancode;
  (void)mods;

  if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}
void Renderer::FPS() {
  currTime = glfwGetTime();
  double diff = currTime - prevTime;
  frames++;
  if (diff >= 1.0 / 30.0) {
    std::string FPS = std::to_string((1.0 / diff) * frames);
    glfwSetWindowTitle(window, FPS.c_str());
    prevTime = currTime;
    frames = 0;
  }
}

void resizeCallback(GLFWwindow *window, int width, int height) {
  (void)width;
  (void)height;
  auto app = reinterpret_cast<Renderer *>(glfwGetWindowUserPointer(window));
  app->resize();
}

} // namespace Renderer
