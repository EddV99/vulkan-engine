/**
 * @file util.hpp
 *
 * @brief header file for util class
 *
 */

#pragma once

#include <string>
#include <vector>

namespace Util {
std::vector<char> readFile(const std::string &filename);
void Error(const std::string &message);
} // namespace Util
