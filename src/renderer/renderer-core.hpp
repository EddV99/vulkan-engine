#pragma once

#include <vector>

#include "../mesh/mesh.hpp"

namespace Renderer {
// ====================================================================================================================
// Frame Data Struct
// ====================================================================================================================
struct FrameData {
  std::vector<Mesh::Mesh> scene;
};
}; // namespace Renderer
