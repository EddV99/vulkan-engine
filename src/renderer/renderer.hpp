/**
 * @file renderer.hpp
 *
 * @brief header file for renderer class
 *
 * @details Implements the renderer using Vulkan.
 */

#pragma once


#include "renderer-core.hpp"

#include "renderer-vulkan.hpp"
#include <GLFW/glfw3.h>

#include "../game/scene.hpp"

namespace Renderer {
class Renderer {
public:
  Renderer(int w, int h, Game::Scene &scene);
  bool running();
  void poll();
  void draw();
  void draw(FrameData frame);
  void resize();
  void FPS();

private:
  static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
  static void resizeCallback(GLFWwindow *window, int width, int height);

  RendererVulkan rendererbackend;
  GLFWwindow *window;
  Game::Scene scene;
  int WIDTH, HEIGHT;

  double prevTime = 0;
  double currTime = 0;
  uint32_t frames = 0;
};

} // namespace Renderer
