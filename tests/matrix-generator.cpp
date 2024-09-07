/**
 * @brief A class to produce many matrices
 *        and the results of doing some matrix operations
 *
 */

#include "../vendor/Eigen/Dense"
#include "testDataGeneratorHelper.hpp"
#include <fstream>
#include <iostream>
#include <string>

// A count of the number of matrices to generate
#define NUMBER_TO_GENERATE 10000
// The max value for entries in matrix
#define MAX_RANGE 100
// The min value for entries in matrix
#define MIN_RANGE -100
// Size of matrix (MATRIX_SIZE x MATRIX_SIZE)
#define MATRIX_SIZE 4
// If binary operation we need two matrices
#define IS_BINARY_OPERATION true
// Location to put the file
const std::string LOC = "files/";
// Name of file to work on/create
const std::string NAME = LOC + "matrix-multiplication";

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
  Eigen::Matrix4f A;
  helper.fillMatrix4x4(A);
  if (IS_BINARY_OPERATION) {
    Eigen::Matrix4f B;
    helper.fillMatrix4x4(B);

    helper.twoMatrixToFile(f, A, B);
    Eigen::Matrix4f C = A * B;
    helper.singleMatrixToFile(r, C);
  } else {
    helper.singleMatrixToFile(f, A);
    Eigen::Matrix4f C = A;
    helper.singleMatrixToFile(r, C);
  }
}
