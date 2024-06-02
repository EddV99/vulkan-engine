/**
 * @brief Entry point of the program
 */
#include "mesh/mesh.hpp"
#include "renderer/renderer.hpp"
#include <exception>
#include <iostream>

int main(void) {
  try {
    Mesh::Scene scene;
    Mesh::Mesh mesh;
    mesh.loadOBJFile("../res/cube/cube.obj");
    scene.push_back(mesh);
    Renderer::Renderer renderer(800, 800, scene);
    

    while (renderer.running()) {
      renderer.draw();

      renderer.poll();
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
    return 0;
  }

  return 0;
}
