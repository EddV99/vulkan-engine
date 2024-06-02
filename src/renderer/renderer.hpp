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
#include "../mesh/mesh.hpp"

namespace Renderer {
class Renderer {
public:
  Renderer(int w, int h, Mesh::Scene scene);
  bool running();
  void poll();
  void draw();
  void draw(FrameData frame);
  void resize();

private:
  RendererVulkan rendererbackend;
  std::vector<Mesh::Mesh> scene;
  GLFWwindow *window;
  int WIDTH, HEIGHT;
};

void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods);

void resizeCallback(GLFWwindow *window, int width, int height);
} // namespace Renderer
