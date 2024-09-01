#include "../src/math/matrix.hpp"
#include "../src/util/util.hpp"

#include <cstdio>
#include <fstream>
#include <gtest/gtest.h>

void loadMatricesData(Math::Matrix4 &A, Math::Matrix4 &B, std::ifstream &data);
void loadMatricesData(Math::Matrix4 &A, Math::Matrix4 &B, std::ifstream &data) {
  bool isA = true;
  std::string line;
  getline(data, line);

  while (!line.empty()) {
    std::string n = "";
    for (int i = 0; i < line.length(); i++) {
      if (line[i] == ' ') {
      }
      n += line[i];
    }
    getline(data, line);
  }
}

TEST(RandomMatrices, Multiplication) {
  std::string name = "../../tests/files/matrix-multiplication.data";
  std::ifstream data(name);
  if (!data.is_open())
    Util::Error("Can't open file: " + name);

  name = "../../tests/files/matrix-multiplication.res";
  std::ifstream res(name);
  if (!res.is_open())
    Util::Error("Can't open file: " + name);

  Math::Matrix4 A;
  Math::Matrix4 B;
  loadMatricesData(A, B, data);
}
