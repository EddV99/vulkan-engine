/**
 * @brief Entry point of the program
 */
#include "game/scene.hpp"
#include "renderer/renderer.hpp"
#include "util/defines.hpp"

#include <exception>
#include <iostream>

int main(void) {
  std::string cubePath = "res/cube/cube.obj";
  std::string teapotPath = "res/teapot/teapot.obj";
  std::string spherePath = "res/sphere/sphere.obj";

  try {
    std::vector<Game::ModelInfo> models;
    Game::ModelInfo sphere;
    sphere.meshFilePath = spherePath;
    sphere.scale = {1.0, 1.0, 1.0};
    sphere.position = {0.0, 0.0, 0.0};
    sphere.rotation = {0.0, 0, 0.0};
    sphere.textureFilePath = "";

    Game::ModelInfo teapot;
    teapot.meshFilePath = teapotPath;
    teapot.scale = {1.0, 1.0, 1.0};
    teapot.position = {0.0, 0.0, 0.0};
    teapot.rotation = {0.0, 0, 0.0};
    teapot.textureFilePath = "";

    models.push_back(sphere);
    models.push_back(teapot);

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
