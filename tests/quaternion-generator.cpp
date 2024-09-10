#include "../vendor/Eigen/Geometry"
#include "testDataGeneratorHelper.hpp"
#include <fstream>
#include <iostream>
#include <string>

// A count of the number of quaternions to generate
#define NUMBER_TO_GENERATE 10000
// The min value for values in entry
#define MIN_RANGE -100
// The max value for values in entry
#define MAX_RANGE 100
// If it is a binary operation we need two quaternions per line
#define IS_BINARY_OPERATION true
const std::string LOC = "files/";
// Name of file to work on/create
const std::string NAME = LOC + "quaternion-rotation";

TestDataGeneratorHelper helper(MIN_RANGE, MAX_RANGE);

void entry(std::fstream &f, std::fstream &r);
int main() {
  std::fstream f;
  f.open(NAME + "-data.txt", std::ios::out | std::ios::app);

  std::fstream r;
  r.open(NAME + "-res.txt", std::ios::out | std::ios::app);

  for (int i = 0; i < NUMBER_TO_GENERATE; i++)
    entry(f, r);
}

void entry(std::fstream &f, std::fstream &r) {
  Eigen::Quaternionf A;
  helper.fillQuaternion(A);

  if (IS_BINARY_OPERATION) {
    // some binary expression here
    A.normalize();
    Eigen::Vector3f B;
    helper.fillVector3(B);
    helper.quaternionAndVector3ToFile(f, A, B);
    Eigen::Vector3f C = A * B;
    helper.singleVectorToFile(r, C);
  } else {
    // some unary expression here
    helper.singleQuaternionToFile(f, A);
    Eigen::Quaternionf C = A.normalized();
    helper.singleQuaternionToFile(r, C);
  }
}
