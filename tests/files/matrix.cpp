#include "../../vendor/Eigen/Dense"
#include <fstream>
#include <iostream>
#include <random>
#include <string>

// A count of the number of matrices to generate
#define NUMBER_TO_GENERATE 10000
// The max range for entries in matrix [0, MAX_RANGE]
#define MAX_RANGE 100
// Size of matrix (MATRIX_SIZE x MATRIX_SIZE)
#define MATRIX_SIZE 4
// Name of file to work on/create
const std::string NAME = "matrix-multiplication";

void entry(std::fstream &f, std::fstream &r);
int main() {
  std::fstream f;
  f.open(NAME + ".data", std::ios::out | std::ios::app);

  std::fstream r;
  r.open(NAME + ".res", std::ios::out | std::ios::app);

  for (int i = 0; i < NUMBER_TO_GENERATE; i++)
    entry(f, r);
}

void entry(std::fstream &f, std::fstream &r) {
  std::random_device rd;  // a seed source for the random number engine
  std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
  std::uniform_int_distribution<> distrib(0, MAX_RANGE);

  Eigen::Matrix4f A;
  Eigen::Matrix4f B;
  A.setZero();
  B.setZero();

  int run = 0;
  while (run < MATRIX_SIZE) {
    for (int i = 0; i < MATRIX_SIZE; i++) {
      float x = distrib(gen);
      if (i + 1 < MATRIX_SIZE)
        f << x << " ";
      else
        f << x << "|";

      A(run, i) = x;
    }
    for (int i = 0; i < MATRIX_SIZE; i++) {
      float x = distrib(gen);
      if (i + 1 < MATRIX_SIZE)
        f << x << " ";
      else
        f << x << "\n";

      B(run, i) = x;
    }
    run++;
  }

  Eigen::Matrix4f C = A * B;

  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      if (j + 1 < MATRIX_SIZE)
        r << C(i, j) << " ";
      else
        r << C(i, j);
    }
    r << "\n";
  }
  r << "\n";
  f << "\n";
}
