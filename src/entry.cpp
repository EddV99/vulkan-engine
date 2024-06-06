/**
 * @brief Entry point of the program
 */
#include "game/object.hpp"
#include "game/scene.hpp"
#include "renderer/renderer.hpp"

#include <exception>
#include <iostream>

int main(void) {
  try {

    Game::Object obj("res/cube/cube.obj");
    std::vector<Game::Object> objs;
    objs.push_back(obj);
    Game::Scene scene(objs);

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
