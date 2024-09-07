#include "../vendor/Eigen/Geometry"
#include <fstream>
#include <random>

class TestDataGeneratorHelper {
public:
  TestDataGeneratorHelper();
  TestDataGeneratorHelper(int min, int max);

  void fillMatrix4x4(Eigen::Matrix4f &A);
  void singleMatrixToFile(std::fstream &file, const Eigen::Matrix4f &A);
  void twoMatrixToFile(std::fstream &file, const Eigen::Matrix4f &A, const Eigen::Matrix4f &B);

  void fillQuaternion(Eigen::Quaternionf &A);
  void singleQuaternionToFile(std::fstream &file, const Eigen::Quaternionf &A);
  void twoQuaternionToFile(std::fstream &file, const Eigen::Quaternionf &A, const Eigen::Quaternionf &B);

private:
  std::random_device seed;
  std::mt19937 gen;
  std::uniform_int_distribution<> distrib;

  std::vector<float> readLineOfFloatsInFile(std::ifstream &file);
};
