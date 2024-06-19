/**
 * @file util.cpp
 */

#include "util.hpp"

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace Util {
std::vector<char> readFile(const std::string &filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("failed to open file");
  }

  size_t fileSize = (size_t)file.tellg();
  std::vector<char> bytes(fileSize);

  file.seekg(0);
  file.read(bytes.data(), fileSize);

  return bytes;
}

void Error(const std::string &message) { throw std::runtime_error(message); }

f32 randomFloat(f32 lo, f32 hi) { return lo + static_cast<f32>(rand() / (static_cast<f32>(RAND_MAX / (hi - lo)))); }

} // namespace Util
