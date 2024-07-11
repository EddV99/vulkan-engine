/**
 * @brief Entry point of the program
 */
#include "game/object.hpp"
#include "game/scene.hpp"
#include "math/vector.hpp"
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
    sphere.scale = {0.5, 0.5, 0.5};
    sphere.position = {-10.0, 10.0, 0.0};
    sphere.rotation = {0.0, 0, 0.0};
    sphere.textureFilePath = "";
    sphere.renderMode = Game::Object::RenderMode::BLINN_SHADING;

    Game::ModelInfo cube;
    cube.meshFilePath = cubePath;
    cube.scale = {18.5, 18.5, 18.5};
    cube.position = {-10.0, 0.0, 0.0};
    cube.rotation = {0.0, 0.0, 0.0};
    cube.textureFilePath = "";
    cube.renderMode = Game::Object::RenderMode::ENVIRONMENT_MAP;

    Game::ModelInfo teapot;
    teapot.meshFilePath = teapotPath;
    teapot.scale = {0.5, 0.5, 0.5};
    teapot.position = {0.0, 0.0, 0.0};
    teapot.rotation = {0.0, 0.0, 0.0};
    teapot.textureFilePath = "";
    teapot.renderMode = Game::Object::RenderMode::BLINN_SHADING;

    Game::ModelInfo sphere2;
    sphere2.meshFilePath = spherePath;
    sphere2.scale = {0.5, 0.5, 0.5};
    sphere2.position = {10.0, 10.0, 0.0};
    sphere2.rotation = {0.0, 0, 0.0};
    sphere2.textureFilePath = "";
    sphere2.renderMode = Game::Object::RenderMode::ENVIRONMENT_MAP;

    Game::ModelInfo cube2;
    cube2.meshFilePath = cubePath;
    cube2.scale = {18.5, 18.5, 18.5};
    cube2.position = {10.0, 0.0, 0.0};
    cube2.rotation = {0.0, 0.0, 0.0};
    cube2.textureFilePath = "";
    cube2.renderMode = Game::Object::RenderMode::BLINN_SHADING;

    Game::ModelInfo teapot2;
    teapot2.meshFilePath = teapotPath;
    teapot2.scale = {0.5, 0.5, 0.5};
    teapot2.position = {-10.0, -20.0, 0.0};
    teapot2.rotation = {0.0, 0.0, 0.0};
    teapot2.textureFilePath = "";
    teapot2.renderMode = Game::Object::RenderMode::ENVIRONMENT_MAP;

    models.push_back(teapot);
    models.push_back(sphere);
    models.push_back(cube);

    models.push_back(teapot2);
    models.push_back(sphere2);
    models.push_back(cube2);

    Game::Scene scene(models);

    Renderer::Renderer renderer(SCREEN_WIDTH, SCREEN_HEIGHT, scene);
    while (renderer.running()) {
      renderer.FPS();
      renderer.draw();

      renderer.poll();
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
    return -1;
  }

  return 0;
}
