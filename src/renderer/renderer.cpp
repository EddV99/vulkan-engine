#include <GLFW/glfw3.h>

#include "../game/scene.hpp"
#include "../util/util.hpp"
#include "input.hpp"
#include "renderer.hpp"

namespace Renderer {

Renderer::Renderer(int w, int h, Game::Scene &scene) {
  WIDTH = w;
  HEIGHT = h;

  mx = w / 2.0;
  my = h / 2.0;

  state = State::RUNNING;

  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  /* glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE); */

  this->window = glfwCreateWindow(WIDTH, HEIGHT, "Beast Gohan", nullptr, nullptr);
  this->scene = scene;
  this->scene.init();

  glfwSetWindowUserPointer(window, this);
  glfwSetKeyCallback(window, keyCallback);
  glfwSetCursorPosCallback(window, mousePointerCallback);
  glfwSetFramebufferSizeCallback(window, resizeCallback);

  rendererbackend.init(this->window, this->scene);
}
void Renderer::draw() {
  if (state != State::RUNNING)
    return;

  f32 speed = 0.0007;
  for (auto &obj : scene.objects) {
    f32 dx = Util::randomFloat(0.0, 360.0) * speed;
    f32 dy = Util::randomFloat(0.0, 360.0) * speed;
    f32 dz = Util::randomFloat(0.0, 360.0) * speed;
    obj.moveRotation({dx, dy, dz});
  }

  rendererbackend.drawScene();
}

void Renderer::poll() {
  if (state != State::RUNNING)
    return;

  glfwPollEvents();
  handleInput();
}

bool Renderer::running() { return state == State::RUNNING; }

void Renderer::resize() {
  if (state != State::RUNNING)
    return;

  rendererbackend.resize();
}

void Renderer::FPS() {
  if (state != State::RUNNING)
    return;

  currTime = glfwGetTime();
  double diff = currTime - prevTime;
  frames++;
  if (diff >= 1.0 / 30.0) {
    std::string FPS = std::to_string((1.0 / diff) * frames);
    glfwSetWindowTitle(window, FPS.c_str());
    prevTime = currTime;
    frames = 0;
  }
}

void Renderer::handleInput() {
  f32 speed = 0.1;

  if (input.isPressed(Keys::KEY_W)) {
    scene.camera.movePositionZ(-1.0 * speed);
  }
  if (input.isPressed(Keys::KEY_A)) {
    scene.camera.movePositionX(-1.0 * speed);
  }
  if (input.isPressed(Keys::KEY_S)) {
    scene.camera.movePositionZ(1.0 * speed);
  }
  if (input.isPressed(Keys::KEY_D)) {
    scene.camera.movePositionX(1.0 * speed);
  }
  if (input.isPressed(Keys::KEY_C)) {
    scene.camera.toggleFreecam();
    input.setUnpressed(Keys::KEY_C);
  }
}
void Renderer::mousePointerCallback(GLFWwindow *window, double x, double y) {
  Renderer *app = static_cast<Renderer *>(glfwGetWindowUserPointer(window));

  if (app->state != State::RUNNING)
    return;

  double dx = x - app->mx;
  double dy = y - app->my;

  app->mx = x;
  app->my = y;

  double sens = 0.1;

  dx *= sens;
  dy *= sens;

  app->scene.camera.ax += dx;
  app->scene.camera.ay += dy;
}

void Renderer::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  (void)scancode;
  (void)mods;

  Renderer *app = static_cast<Renderer *>(glfwGetWindowUserPointer(window));

  if (app->state != State::RUNNING)
    return;

  if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
    app->state = State::STOPPED;
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }

  if (key == GLFW_KEY_W && action == GLFW_PRESS) {
    app->input.setPressed(Keys::KEY_W);
  }
  if (key == GLFW_KEY_A && action == GLFW_PRESS) {
    app->input.setPressed(Keys::KEY_A);
  }
  if (key == GLFW_KEY_S && action == GLFW_PRESS) {
    app->input.setPressed(Keys::KEY_S);
  }
  if (key == GLFW_KEY_D && action == GLFW_PRESS) {
    app->input.setPressed(Keys::KEY_D);
  }
  if (key == GLFW_KEY_C && action == GLFW_PRESS) {
    app->input.setPressed(Keys::KEY_C);
  }

  if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
    app->input.setUnpressed(Keys::KEY_W);
  }
  if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
    app->input.setUnpressed(Keys::KEY_A);
  }
  if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
    app->input.setUnpressed(Keys::KEY_S);
  }
  if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
    app->input.setUnpressed(Keys::KEY_D);
  }
  if (key == GLFW_KEY_C && action == GLFW_RELEASE) {
    app->input.setUnpressed(Keys::KEY_C);
  }
}

void Renderer::resizeCallback(GLFWwindow *window, int width, int height) {
  (void)width;
  (void)height;
  auto app = reinterpret_cast<Renderer *>(glfwGetWindowUserPointer(window));

  if (app->state != State::RUNNING)
    return;

  app->resize();
}

} // namespace Renderer
