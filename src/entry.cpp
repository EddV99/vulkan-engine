/**
 * @brief Entry point of the program
 */
#include "game/object.hpp"
#include "game/scene.hpp"
#include "renderer/renderer.hpp"

#include <exception>
#include <iostream>

int main(void) {
  /* std::string meshFilePath = "res/cube/cube.obj"; */
  std::string meshFilePath = "res/teapot/teapot.obj";
  /* std::string meshFilePath = "res/sphere/sphere.obj"; */

  try {
    std::vector<Game::ModelInfo> models;
    Game::ModelInfo model;
    model.meshFilePath = meshFilePath;
    model.scale = {1.0, 1.0, 1.0};
    model.position = {0.0, 0.0, 0.0};
    model.rotation = {0.0, 0.0, 0.0};
    model.textureFilePath = "";
    models.push_back(model);

    Game::Scene scene(models);

    Renderer::Renderer renderer(1000, 800, scene);
    while (renderer.running()) {
      renderer.FPS();
      renderer.draw();

      renderer.poll();
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
    return 0;
  }

  return 0;
}
