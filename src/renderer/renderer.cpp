#include <GLFW/glfw3.h>

#include "../game/scene.hpp"
#include "../util/util.hpp"
#include "input.hpp"
#include "renderer.hpp"

#include <cmath>

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
  this->window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Engine", nullptr, nullptr);
  this->scene = scene;
  this->scene.init();

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetWindowUserPointer(window, this);
  glfwSetKeyCallback(window, keyCallback);
  glfwSetCursorPosCallback(window, mousePointerCallback);
  glfwSetFramebufferSizeCallback(window, resizeCallback);

  rendererbackend.init(this->window, WIDTH, HEIGHT);
  rendererbackend.createAssets(&this->scene);
  rendererbackend.createPipelines();
}

void Renderer::draw() {
  if (state != State::RUNNING)
    return;

  f32 speed = 0.0007;
  if (spin) {
    for (auto &obj : scene.objects) {
      f32 dx = Util::randomFloat(0.0, 360.0) * speed;
      f32 dy = Util::randomFloat(0.0, 360.0) * speed;
      f32 dz = Util::randomFloat(0.0, 360.0) * speed;
      obj.moveRotation({dx, dy, dz});
    }
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
  f32 speed = 0.002;

  if (input.isPressed(Keys::KEY_W)) {
    scene.camera.movePosition(scene.camera.getForwardVector() * (speed * -1.0));
    scene.camera.update();
  }
  if (input.isPressed(Keys::KEY_A)) {
    scene.camera.movePosition(scene.camera.getRightVector() * (speed * -1.0));
    scene.camera.update();
  }
  if (input.isPressed(Keys::KEY_S)) {
    scene.camera.movePosition(scene.camera.getForwardVector() * speed);
    scene.camera.update();
  }
  if (input.isPressed(Keys::KEY_D)) {
    scene.camera.movePosition(scene.camera.getRightVector() * speed);
    scene.camera.update();
  }
  if (input.isPressed(Keys::KEY_C)) {
    scene.camera.toggleFreecam();
    input.setUnpressed(Keys::KEY_C);
    scene.camera.update();
  }
  if (input.isPressed(Keys::KEY_R)) {
    spin = !spin;
    input.setUnpressed(Keys::KEY_R);
    scene.camera.update();
  }
}

void Renderer::mousePointerCallback(GLFWwindow *window, double x, double y) {
  Renderer *app = static_cast<Renderer *>(glfwGetWindowUserPointer(window));

  if (app->state != State::RUNNING || !app->scene.camera.isFreecam())
    return;

  if (app->firstMouse) {
    app->mx = x;
    app->my = y;
    app->firstMouse = false;
  }

  double dx = x - app->mx;
  double dy = app->my - y;

  app->mx = x;
  app->my = y;

  double sens = 0.5;

  dx *= sens;
  dy *= sens;

  app->scene.camera.yaw += dx;
  app->scene.camera.pitch += dy;

  if (app->scene.camera.pitch > 89.0)
    app->scene.camera.pitch = 89.0;
  if (app->scene.camera.pitch < -89.0)
    app->scene.camera.pitch = -89.0;

  /*app->scene.camera.direction.x =*/
  /*    std::cos(TO_RADIANS(app->scene.camera.yaw)) * std::cos(TO_RADIANS(app->scene.camera.pitch));*/
  /*app->scene.camera.direction.y = std::sin(TO_RADIANS(app->scene.camera.pitch));*/
  /*app->scene.camera.direction.z =*/
  /*    std::cos(TO_RADIANS(app->scene.camera.pitch)) * std::sin(TO_RADIANS(app->scene.camera.yaw));*/

  app->scene.camera.direction.x = std::cos(TO_RADIANS(app->scene.camera.yaw));
  app->scene.camera.direction.z = std::sin(TO_RADIANS(app->scene.camera.yaw));
  app->scene.camera.direction.y = 0;

  app->scene.camera.update(); // might be a better place to do an update for mouse movement
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
  if (key == GLFW_KEY_R && action == GLFW_PRESS) {
    app->input.setPressed(Keys::KEY_R);
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
  if (key == GLFW_KEY_R && action == GLFW_RELEASE) {
    app->input.setUnpressed(Keys::KEY_R);
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
