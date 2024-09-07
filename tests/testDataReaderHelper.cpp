#include "testDataReaderHelper.hpp"
#include <vector>

bool readSingleQuaternionFromFile(std::ifstream &file, Math::Quaternion &A) {
  if (file.eof())
    return false;

  std::vector<float> line = readLineOfFloatsInFile(file);

  if(line.empty())
    return false;

  A.w = line[0];
  A.v.x = line[1];
  A.v.y = line[2];
  A.v.z = line[3];

  return true;
}

bool readTwoQuaternionFromFile(std::ifstream &file, Math::Quaternion &A, Math::Quaternion &B) {
  if (file.eof())
    return false;

  std::vector<float> line = readLineOfFloatsInFile(file);

  if(line.empty())
    return false;

  A.w = line[0];
  A.v.x = line[1];
  A.v.y = line[2];
  A.v.z = line[3];

  B.w = line[4];
  B.v.x = line[5];
  B.v.y = line[6];
  B.v.z = line[7];

  return true;
}

bool readSingleMatrixFromFile(std::ifstream &file, Math::Matrix4 &A) {
  if (file.eof())
    return false;

  std::vector<float> line = readLineOfFloatsInFile(file);

  if(line.empty())
    return false;

  A.set(0, 0, line[0]);
  A.set(0, 1, line[1]);
  A.set(0, 2, line[2]);
  A.set(0, 3, line[3]);

  line = readLineOfFloatsInFile(file);
  A.set(1, 0, line[0]);
  A.set(1, 1, line[1]);
  A.set(1, 2, line[2]);
  A.set(1, 3, line[3]);

  line = readLineOfFloatsInFile(file);
  A.set(2, 0, line[0]);
  A.set(2, 1, line[1]);
  A.set(2, 2, line[2]);
  A.set(2, 3, line[3]);

  line = readLineOfFloatsInFile(file);
  A.set(3, 0, line[0]);
  A.set(3, 1, line[1]);
  A.set(3, 2, line[2]);
  A.set(3, 3, line[3]);

  return true;
}

bool readTwoMatrixFromFile(std::ifstream &file, Math::Matrix4 &A, Math::Matrix4 &B) {
  if (file.eof())
    return false;
  std::vector<float> line = readLineOfFloatsInFile(file);

  if(line.empty())
    return false;

  A.set(0, 0, line[0]);
  A.set(0, 1, line[1]);
  A.set(0, 2, line[2]);
  A.set(0, 3, line[3]);

  B.set(0, 0, line[4]);
  B.set(0, 1, line[5]);
  B.set(0, 2, line[6]);
  B.set(0, 3, line[7]);

  line = readLineOfFloatsInFile(file);
  A.set(1, 0, line[0]);
  A.set(1, 1, line[1]);
  A.set(1, 2, line[2]);
  A.set(1, 3, line[3]);

  B.set(1, 0, line[4]);
  B.set(1, 1, line[5]);
  B.set(1, 2, line[6]);
  B.set(1, 3, line[7]);

  line = readLineOfFloatsInFile(file);
  A.set(2, 0, line[0]);
  A.set(2, 1, line[1]);
  A.set(2, 2, line[2]);
  A.set(2, 3, line[3]);

  B.set(2, 0, line[4]);
  B.set(2, 1, line[5]);
  B.set(2, 2, line[6]);
  B.set(2, 3, line[7]);

  line = readLineOfFloatsInFile(file);
  A.set(3, 0, line[0]);
  A.set(3, 1, line[1]);
  A.set(3, 2, line[2]);
  A.set(3, 3, line[3]);

  B.set(3, 0, line[4]);
  B.set(3, 1, line[5]);
  B.set(3, 2, line[6]);
  B.set(3, 3, line[7]);

  return true;
}

std::vector<float> readLineOfFloatsInFile(std::ifstream &file) {
  std::string line;
  std::getline(file, line);

  if (line.empty())
    return {};

  std::vector<float> result;
  std::string num = "";
  for (auto &ch : line) {
    if (ch == ' ' || ch == '|') {
      result.push_back(std::stof(num));
      num.clear();
    } else {
      num += ch;
    }
  }
  result.push_back(std::stof(num));

  return result;
}
