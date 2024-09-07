#include "../src/math/matrix.hpp"
#include "../src/math/quaternion.hpp"
#include <fstream>
#include <vector>

bool readSingleQuaternionFromFile(std::ifstream &file, Math::Quaternion &A);
bool readTwoQuaternionFromFile(std::ifstream &file, Math::Quaternion &A, Math::Quaternion &B);

bool readSingleMatrixFromFile(std::ifstream &file, Math::Matrix4 &A);
bool readTwoMatrixFromFile(std::ifstream &file, Math::Matrix4 &A, Math::Matrix4 &B);
std::vector<float> readLineOfFloatsInFile(std::ifstream &file);
