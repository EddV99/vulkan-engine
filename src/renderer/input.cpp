#include "input.hpp"

namespace Renderer {
bool Input::isPressed(Keys key) { return keys[key]; }
void Input::setPressed(Keys key) { keys[key] = true; }
void Input::setUnpressed(Keys key) { keys[key] = false; }
}; // namespace Renderer
