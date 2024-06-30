#pragma once

namespace Renderer {

#define DEFINED_KEYS_COUNT 6
enum Keys { KEY_W = 0, KEY_A, KEY_S, KEY_D, KEY_C, KEY_R };

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
