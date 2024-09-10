#include "testDataGeneratorHelper.hpp"
#include <fstream>
#include <random>

TestDataGeneratorHelper::TestDataGeneratorHelper() {
  gen.seed(seed());
  std::uniform_int_distribution<>::param_type a(0, 100);
  distrib.param(a);
}

TestDataGeneratorHelper::TestDataGeneratorHelper(int min, int max) {
  gen.seed(seed());
  std::uniform_int_distribution<>::param_type a(min, max);
  distrib.param(a);
}

void TestDataGeneratorHelper::fillMatrix4x4(Eigen::Matrix4f &A) {
  for (int i = 0; i < A.size(); i++)
    A(i) = distrib(gen);
}

void TestDataGeneratorHelper::singleMatrixToFile(std::fstream &file, const Eigen::Matrix4f &A) {
  for (int row = 0; row < A.rows(); row++) {
    for (int col = 0; col < A.cols(); col++) {
      file << A(row, col);
      if (col < A.cols() - 1)
        file << " ";
    }
    file << "\n";
  }
}

void TestDataGeneratorHelper::twoMatrixToFile(std::fstream &file, const Eigen::Matrix4f &A, const Eigen::Matrix4f &B) {
  for (int row = 0; row < A.rows(); row++) {
    for (int col = 0; col < A.cols(); col++) {
      file << A(row, col);
      if (col < A.cols() - 1)
        file << " ";
    }
    file << "|";
    for (int col = 0; col < B.cols(); col++) {
      file << B(row, col);
      if (col < B.cols() - 1)
        file << " ";
    }
    file << "\n";
  }
}

void TestDataGeneratorHelper::fillQuaternion(Eigen::Quaternionf &A) {
  A.w() = distrib(gen);
  A.x() = distrib(gen);
  A.y() = distrib(gen);
  A.z() = distrib(gen);
}
void TestDataGeneratorHelper::singleQuaternionToFile(std::fstream &file, const Eigen::Quaternionf &A) {
  file << A.w() << " " << A.x() << " " << A.y() << " " << A.z() << "\n";
}
void TestDataGeneratorHelper::twoQuaternionToFile(std::fstream &file, const Eigen::Quaternionf &A,
                                                  const Eigen::Quaternionf &B) {
  file << A.w() << " " << A.x() << " " << A.y() << " " << A.z() << "|";
  file << B.w() << " " << B.x() << " " << B.y() << " " << B.z() << "\n";
}

void TestDataGeneratorHelper::quaternionAndVector3ToFile(std::fstream &file, const Eigen::Quaternionf &A,
                                                         const Eigen::Vector3f &B) {
  file << A.w() << " " << A.x() << " " << A.y() << " " << A.z() << "|";
  file << B.x() << " " << B.y() << " " << B.z() << "\n";
}

void TestDataGeneratorHelper::fillVector3(Eigen::Vector3f &A) {
  A.x() = distrib(gen);
  A.y() = distrib(gen);
  A.z() = distrib(gen);
}

void TestDataGeneratorHelper::singleVectorToFile(std::fstream &file, const Eigen::Vector3f &A) {
  file << A.x() << " " << A.y() << " " << A.z() << "\n";
}
