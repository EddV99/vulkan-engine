#include "../src/math/matrix.hpp"
#include "../src/math/quaternion.hpp"
#include "../src/util/util.hpp"

#include <cstdio>
#include <fstream>
#include <gtest/gtest.h>
#include <sys/types.h>

bool loadMatricesData(Math::Matrix4 &A, Math::Matrix4 &B, std::ifstream &data);
bool loadMatrixResult(Math::Matrix4 &A, std::ifstream &data);

bool loadQuaternionData(Math::Quaternion &A, Math::Quaternion &B, std::ifstream &data);
bool loadQuaternionResult(Math::Quaternion &A, std::ifstream &data);

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
    EXPECT_EQ(A * B, C);
  }
}

TEST(RandomQuaternions10K, Multiplication) {
  std::string name = "../../tests/files/quaternion-multiplication.data";
  std::ifstream data(name);

  if (!data.is_open())
    Util::Error("Can't open file: " + name);

  name = "../../tests/files/quaternion-multiplication.res";

  std::ifstream res(name);
  if (!res.is_open())
    Util::Error("Can't open file: " + name);

  Math::Quaternion A;
  Math::Quaternion B;
  Math::Quaternion C;

  while (loadQuaternionData(A, B, data) && loadQuaternionResult(C, res)) {
    EXPECT_EQ(A * B, C);
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

bool loadQuaternionData(Math::Quaternion &A, Math::Quaternion &B, std::ifstream &data) {
  int i = 0;
  std::string line;
  bool isA = true;
  int x = 0;
  getline(data, line);
  if (!line.empty() && !data.eof()) {
    std::string num = "";
    i = 0;
    while (i <= line.size()) {
      if (i == line.size() || line[i] == ' ') {
        if (isA) {
          if (x == 0)
            A.w = std::stof(num);
          else if (x == 1)
            A.v.x = std::stof(num);
          else if (x == 2)
            A.v.y = std::stof(num);
        } else {
          if (x == 0)
            B.w = std::stof(num);
          else if (x == 1)
            B.v.x = std::stof(num);
          else if (x == 2)
            B.v.y = std::stof(num);
          else
            B.v.z = std::stof(num);
        }
        x++;
        num.clear();
      } else if (line[i] == '|') {
        A.v.z = std::stof(num);
        num.clear();
        isA = false;
        x = 0;
      } else {
        num += line[i];
      }
      i++;
    }

    isA = true;
  }

  if (data.eof() || line.empty())
    return false;

  return true;
}
bool loadQuaternionResult(Math::Quaternion &A, std::ifstream &data) {
  int i = 0;
  std::string line;
  int x = 0;

  getline(data, line);

  if (!line.empty() && !data.eof()) {
    std::string num = "";
    i = 0;
    while (i <= line.size()) {
      if (i == line.size() || line[i] == ' ') {
        if (x == 0)
          A.w = std::stof(num);
        else if (x == 1)
          A.v.x = std::stof(num);
        else if (x == 2)
          A.v.y = std::stof(num);
        else
          A.v.z = std::stof(num);
        x++;
        num.clear();
      } else {
        num += line[i];
      }
      i++;
    }
  }

  if (data.eof() || line.empty())
    return false;

  return true;
}
