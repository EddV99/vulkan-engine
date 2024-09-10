#include "../src/math/matrix.hpp"
#include "../src/math/quaternion.hpp"
#include "../src/util/util.hpp"
#include "testDataReaderHelper.hpp"

#include <cstdio>
#include <fstream>
#include <gtest/gtest.h>
#include <sys/types.h>

float margin = 0.01;

TEST(RandomMatrices10K, Multiplication) {
  std::string name = "../../tests/files/matrix-multiplication-data.txt";
  std::ifstream data(name);

  if (!data.is_open())
    Util::Error("Can't open file: " + name);

  name = "../../tests/files/matrix-multiplication-res.txt";

  std::ifstream res(name);
  if (!res.is_open())
    Util::Error("Can't open file: " + name);

  Math::Matrix4 A;
  Math::Matrix4 B;
  Math::Matrix4 C;
  while (readTwoMatrixFromFile(data, A, B) && readSingleMatrixFromFile(res, C)) {
    EXPECT_EQ(A * B, C);
  }
}

TEST(RandomQuaternions10K, Multiplication) {
  std::string name = "../../tests/files/quaternion-multiplication-data.txt";
  std::ifstream data(name);

  if (!data.is_open())
    Util::Error("Can't open file: " + name);

  name = "../../tests/files/quaternion-multiplication-res.txt";

  std::ifstream res(name);
  if (!res.is_open())
    Util::Error("Can't open file: " + name);

  Math::Quaternion A;
  Math::Quaternion B;
  Math::Quaternion C;

  while (readTwoQuaternionFromFile(data, A, B) && readSingleQuaternionFromFile(res, C)) {
    EXPECT_EQ(A * B, C);
  }
}

TEST(RandomQuaternions10K, Normalize) {
  std::string name = "../../tests/files/quaternion-normalize-data.txt";
  std::ifstream data(name);

  if (!data.is_open())
    Util::Error("Can't open file: " + name);

  name = "../../tests/files/quaternion-normalize-res.txt";

  std::ifstream res(name);
  if (!res.is_open())
    Util::Error("Can't open file: " + name);

  Math::Quaternion A;
  Math::Quaternion C;

  while (readSingleQuaternionFromFile(data, A) && readSingleQuaternionFromFile(res, C)) {
    A.normalize();
    EXPECT_NEAR(A.w, C.w, margin);
    EXPECT_NEAR(A.v.x, C.v.x, margin);
    EXPECT_NEAR(A.v.y, C.v.y, margin);
    EXPECT_NEAR(A.v.z, C.v.z, margin);
  }
}

TEST(RandomQuaternions10K, ToRotationMatrix) {
  std::string name = "../../tests/files/quaternion-to-rotation-matrix-data.txt";
  std::ifstream data(name);

  if (!data.is_open())
    Util::Error("Can't open file: " + name);

  name = "../../tests/files/quaternion-to-rotation-matrix-res.txt";

  std::ifstream res(name);
  if (!res.is_open())
    Util::Error("Can't open file: " + name);

  Math::Quaternion A;
  Math::Matrix4 C;

  while (readSingleQuaternionFromFile(data, A) && readSingleMatrixFromFile(res, C)) {
    Math::Matrix4 D = A.toRotationMatrix();
    for (int r = 0; r < 4; r++)
      for (int c = 0; c < 4; c++)
        EXPECT_NEAR(D(r, c), C(r, c), margin);
  }
}

TEST(RandomQuaternions10K, rotatePoint) {
  std::string name = "../../tests/files/quaternion-rotation-data.txt";
  std::ifstream data(name);

  if (!data.is_open())
    Util::Error("Can't open file: " + name);

  name = "../../tests/files/quaternion-rotation-res.txt";

  std::ifstream res(name);
  if (!res.is_open())
    Util::Error("Can't open file: " + name);

  Math::Quaternion rot;
  Math::Vector3 pos;
  Math::Vector3 newPos;

  while (readQuaternionAndVectorFromFile(data, rot, pos) && readVectorFromFile(res, newPos)) {
    Math::Vector3 r = Math::Quaternion::rotateVector(rot, pos);
    EXPECT_NEAR(r.x, newPos.x, margin);
    EXPECT_NEAR(r.y, newPos.y, margin);
    EXPECT_NEAR(r.z, newPos.z, margin);
  }
}
