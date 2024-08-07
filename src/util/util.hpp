/**
 * @file util.hpp
 *
 * @brief header file for util class
 *
 */

#pragma once

#include "defines.hpp"

#include <string>
#include <vector>

namespace Util {
std::vector<char> readFile(const std::string &filename);
void Error(const std::string &message);
f32 randomFloat(f32 lo, f32 hi);
void loadImage(std::string filePath, unsigned char *&data, int &width, int &height, int &channels);
} // namespace Util
