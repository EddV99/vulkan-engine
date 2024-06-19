/**
 * @file renderer.hpp
 *
 * @brief header file for renderer class
 *
 * @details Implements the renderer using Vulkan.
 */

#pragma once

#include "renderer-vulkan.hpp"
#include <GLFW/glfw3.h>

#include "../game/scene.hpp"
#include "input.hpp"

namespace Renderer {
class Renderer {
public:
  Renderer(int w, int h, Game::Scene &scene);
  bool running();
  void poll();
  void draw();
  void resize();
  void FPS();

private:
  enum State { RUNNING, STOPPED } state;
  static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
  static void resizeCallback(GLFWwindow *window, int width, int height);
  void handleInput();

  RendererVulkan rendererbackend;
  GLFWwindow *window;
  Game::Scene scene;
  int WIDTH, HEIGHT;

  f32 mx = 0, my = 0;

  Input input;

  double prevTime = 0;
  double currTime = 0;
  uint32_t frames = 0;

};

} // namespace Renderer
