#include "../src/math/matrix.hpp"
#include "../src/util/util.hpp"

#include <cstdio>
#include <fstream>
#include <gtest/gtest.h>
#include <sys/types.h>

bool loadMatricesData(Math::Matrix4 &A, Math::Matrix4 &B, std::ifstream &data);
bool loadMatrixResult(Math::Matrix4 &A, std::ifstream &data);

TEST(RandomMatrices10K, Multiplication) {
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
  Math::Matrix4 C;
  while (loadMatricesData(A, B, data) && loadMatrixResult(C, res)) {
    EXPECT_TRUE(A * B == C);
  }
}

bool loadMatrixResult(Math::Matrix4 &C, std::ifstream &data) {
  int r = 0, c = 0;
  int i = 0;
  std::string line;

  getline(data, line);
  while (!line.empty() && !data.eof()) {

    std::string num = "";
    i = 0;

    while (i <= line.size()) {
      if (i == line.size() || line[i] == ' ') {
        C.set(r, c, std::stof(num));
        num.clear();
        c++;
      } else {
        num += line[i];
      }
      i++;
    }
    r++;
    c = 0;
    getline(data, line);
  }

  if (data.eof())
    return false;

  return true;
}

bool loadMatricesData(Math::Matrix4 &A, Math::Matrix4 &B, std::ifstream &data) {
  int r = 0, c = 0;
  int i = 0;
  std::string line;
  bool isA = true;

  getline(data, line);
  while (!line.empty() && !data.eof()) {

    std::string num = "";
    i = 0;
    while (i <= line.size()) {
      if (i == line.size() || line[i] == ' ') {
        if (isA)
          A.set(r, c, std::stof(num));
        else
          B.set(r, c, std::stof(num));
        c++;
        num.clear();
      } else if (line[i] == '|') {
        A.set(r, c, std::stof(num));
        num.clear();
        isA = false;
        c = 0;
      } else {
        num += line[i];
      }
      i++;
    }

    isA = true;
    r++;
    c = 0;
    getline(data, line);
  }

  if (data.eof())
    return false;

  return true;
}
