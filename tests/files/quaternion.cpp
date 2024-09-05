#include "../../vendor/Eigen/Geometry"
#include <fstream>
#include <iostream>
#include <random>
#include <string>

// A count of the number of quaternions to generate
#define NUMBER_TO_GENERATE 10000
// The max range for entries in matrix [0, MAX_RANGE]
#define MAX_RANGE 100
// Name of file to work on/create
const std::string NAME = "quaternion-multiplication";

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

  float i = distrib(gen);
  float j = distrib(gen);
  float k = distrib(gen);
  float w = distrib(gen);
  Eigen::Quaternionf A(w, i, j, k);
  f << w << " " << i << " " << j << " " << k;

  i = distrib(gen);
  j = distrib(gen);
  k = distrib(gen);
  w = distrib(gen);
  Eigen::Quaternionf B(w, i, j, k);
  f << "|" << w << " " << i << " " << j << " " << k << "\n";

  Eigen::Quaternionf C = A * B;
  r << C.w() << " " << C.x() << " " << C.y() << " " << C.z() << "\n";
}
