/**
 * @file renderer.hpp
 *
 * @brief header file for renderer class
 *
 * @details Implements the renderer using Vulkan.
 */

#pragma once

#include "../game/scene.hpp"
#include "../mesh/mesh.hpp"
#include "renderer-core.hpp"
#include "renderer-vulkan.hpp"

namespace Renderer {
class Renderer {
public:
  Renderer(int w, int h, Game::Scene scene);
  bool running();
  void poll();
  void draw();
  void draw(FrameData frame);
  void resize();
  void FPS();

private:
  RendererVulkan rendererbackend;
  std::vector<Mesh::Mesh> scene;
  GLFWwindow *window;
  int WIDTH, HEIGHT;

  double prevTime = 0;
  double currTime = 0;
  uint32_t frames = 0;
};

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

void resizeCallback(GLFWwindow *window, int width, int height);
} // namespace Renderer
