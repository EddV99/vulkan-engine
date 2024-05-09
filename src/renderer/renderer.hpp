/**
 * @file renderer.hpp
 *
 * @brief header file for renderer class
 *
 * @details Implements the renderer using Vulkan.
 */

#pragma once

#include "../util/util.hpp"

namespace Renderer {
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class Renderer {
public:
private:
};
} // namespace Renderer
