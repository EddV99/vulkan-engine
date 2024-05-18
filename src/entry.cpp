/**
 * @brief Entry point of the program
 */
#include "renderer/renderer.hpp"

int main(void) {
  Renderer::Renderer renderer(800, 800);

  while (renderer.running()) {
    renderer.draw();

    renderer.poll();
  }
}
