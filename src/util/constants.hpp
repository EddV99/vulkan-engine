#pragma once

namespace Renderer {
#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

const int MAX_FRAMES_IN_FLIGHT = 2;
} // namespace Renderer
