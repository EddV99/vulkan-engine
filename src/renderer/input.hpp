#pragma once

namespace Renderer {
#define DEFINED_KEYS_COUNT 4
enum Keys { KEY_W = 0, KEY_A, KEY_S, KEY_D };
class Input {
private:
  bool keys[DEFINED_KEYS_COUNT]{false};

public:
  Input() = default;
  ~Input() = default;

  bool isPressed(Keys key);
  void setPressed(Keys key);
  void setUnpressed(Keys key);
};
} // namespace Renderer
