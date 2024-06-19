/**
 * @brief Entry point of the program
 */
#include "game/scene.hpp"
#include "renderer/renderer.hpp"

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
    sphere.scale = {0.5, 0.5, 0.5};
    sphere.position = {-40.0, 40.0, 0.0};
    sphere.rotation = {0.0, 0, 0.0};
    sphere.textureFilePath = "";

    Game::ModelInfo cube;
    cube.meshFilePath = cubePath;
    cube.scale = {18.5, 18.5, 18.5};
    cube.position = {-40.0, 0.0, 0.0};
    cube.rotation = {0.0, 0.0, 0.0};
    cube.textureFilePath = "";

    Game::ModelInfo teapot;
    teapot.meshFilePath = teapotPath;
    teapot.scale = {1.0, 1.0, 1.0};
    teapot.position = {0.0, 0.0, 0.0};
    teapot.rotation = {0.0, 0.0, 0.0};
    teapot.textureFilePath = "";

    models.push_back(teapot);
    /* models.push_back(sphere); */
    /* models.push_back(cube); */

    Game::Scene scene(models);

    Renderer::Renderer renderer(800, 800, scene);
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
