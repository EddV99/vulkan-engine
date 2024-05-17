#include "matrix.hpp"
#include <initializer_list>
#include <iostream>

namespace Math {

void Matrix3::print() {
  for (int r = 0; r < SIZE; r++) {
    for (int c = 0; c < SIZE; c++) {
      std::cout << (*this)(r, c) << " ";
    }
    std::cout << "\n";
  }
  std::cout << "\n";
}
void Matrix4::print() {
  for (int r = 0; r < SIZE; r++) {
    for (int c = 0; c < SIZE; c++) {
      std::cout << (*this)(r, c) << " ";
    }
    std::cout << "\n";
  }
  std::cout << "\n";
}

Matrix3::Matrix3() {
  data[0][0] = 1.0f;
  data[0][1] = 0.0f;
  data[0][2] = 0.0f;

  data[1][0] = 0.0f;
  data[1][1] = 1.0f;
  data[1][2] = 0.0f;

  data[2][0] = 0.0f;
  data[2][1] = 0.0f;
  data[2][2] = 1.0f;
}
Matrix3::Matrix3(f32 value) {
  data[0][0] = value;
  data[0][1] = 0.0f;
  data[0][2] = 0.0f;

  data[1][0] = 0.0f;
  data[1][1] = value;
  data[1][2] = 0.0f;

  data[2][0] = 0.0f;
  data[2][1] = 0.0f;
  data[2][2] = value;
}

Matrix3::Matrix3(Matrix3 const &copy) {
  for (int r = 0; r < SIZE; r++)
    for (int c = 0; c < SIZE; c++)
      this->data[r][c] = copy.data[r][c];
}

Matrix3::Matrix3(f32 v00, f32 v01, f32 v02, //
                 f32 v10, f32 v11, f32 v12, //
                 f32 v20, f32 v21, f32 v22) {
  data[0][0] = v00;
  data[0][1] = v01;
  data[0][2] = v02;

  data[1][0] = v10;
  data[1][1] = v11;
  data[1][2] = v12;

  data[2][0] = v20;
  data[2][1] = v21;
  data[2][2] = v22;
}

void Matrix3::set(i32 row, i32 col, f32 value) { data[row][col] = value; }

f32 Matrix3::operator()(i32 row, i32 col) { return this->data[row][col]; }
f32 Matrix3::operator()(i32 row, i32 col) const { return this->data[row][col]; }

Matrix3 Matrix3::operator+(Matrix3 &right) {
  Matrix3 value(0.0f);
  for (int r = 0; r < SIZE; r++) {
    for (int c = 0; c < SIZE; c++) {
      value.set(r, c, (*this)(r, c) + right(r, c));
    }
  }
  return value;
}

Matrix3 Matrix3::operator-(Matrix3 &right) {
  Matrix3 value(0.0f);
  for (int r = 0; r < SIZE; r++) {
    for (int c = 0; c < SIZE; c++) {
      value.set(r, c, (*this)(r, c) - right(r, c));
    }
  }
  return value;
}

Matrix3 Matrix3::operator*(f32 scalar) {
  Matrix3 value((*this));

  for (int r = 0; r < SIZE; r++)
    for (int c = 0; c < SIZE; c++)
      value.data[r][c] *= scalar;

  return value;
}

Matrix3 Matrix3::operator*(Matrix3 &right) {
  Matrix3 value(0.0f);

  for (int r = 0; r < SIZE; r++)
    for (int c = 0; c < SIZE; c++)
      for (int i = 0; i < SIZE; i++)
        value.set(r, c, value(r, c) + (*this)(r, i) * right(i, c));

  return value;
}

Matrix3 &Matrix3::operator=(Matrix3 other) noexcept {
  for (int r = 0; r < SIZE; r++) {
    for (int c = 0; c < SIZE; c++) {
      this->set(r, c, other(r, c));
    }
  }
  return *this;
}

void Matrix3::transpose() {
  for (int r = 0; r < SIZE; r++) {
    for (int c = 0; c < SIZE; c++) {
      f32 temp = (*this)(r, c);
      this->set(r, c, (*this)(c, r));
      this->set(c, r, temp);
    }
  }
}

f32 Matrix3::determinate() {
  return (*this)(0, 0) * (data[1][1] * data[2][2] - data[2][1] * data[1][2]) -
         (*this)(0, 1) * (data[1][0] * data[2][2] - data[2][0] * data[1][2]) +
         (*this)(0, 2) * (data[1][0] * data[2][1] - data[2][0] * data[1][1]);
}

void Matrix3::inverse() {
  Matrix3 inverted(0.0);

  inverted.data[0][0] = data[1][1] * data[2][2] - data[2][1] * data[1][2];
  inverted.data[0][1] = data[0][2] * data[2][1] - data[2][2] * data[0][1];
  inverted.data[0][2] = data[0][1] * data[1][2] - data[1][1] * data[0][2];

  inverted.data[1][0] = data[1][2] * data[2][0] - data[2][2] * data[1][0];
  inverted.data[1][1] = data[0][0] * data[2][2] - data[2][0] * data[0][2];
  inverted.data[1][2] = data[0][2] * data[1][0] - data[1][2] * data[0][0];

  inverted.data[2][0] = data[1][0] * data[2][1] - data[2][0] * data[1][1];
  inverted.data[2][1] = data[0][1] * data[2][0] - data[2][1] * data[0][0];
  inverted.data[2][2] = data[0][0] * data[1][1] - data[1][0] * data[0][1];

  (*this) = inverted * (1.0f / this->determinate());
}

Matrix4::Matrix4() {
  data[0][0] = 1.0f;
  data[0][1] = 0.0f;
  data[0][2] = 0.0f;
  data[0][3] = 0.0f;

  data[1][0] = 0.0f;
  data[1][1] = 1.0f;
  data[1][2] = 0.0f;
  data[1][3] = 0.0f;

  data[2][0] = 0.0f;
  data[2][1] = 0.0f;
  data[2][2] = 1.0f;
  data[2][3] = 0.0f;

  data[3][0] = 0.0f;
  data[3][1] = 0.0f;
  data[3][2] = 0.0f;
  data[3][3] = 1.0f;
}
Matrix4::Matrix4(f32 value) {
  data[0][0] = value;
  data[0][1] = 0.0f;
  data[0][2] = 0.0f;
  data[0][3] = 0.0f;

  data[1][0] = 0.0f;
  data[1][1] = value;
  data[1][2] = 0.0f;
  data[1][3] = 0.0f;

  data[2][0] = 0.0f;
  data[2][1] = 0.0f;
  data[2][2] = value;
  data[2][3] = 0.0f;

  data[3][0] = 0.0f;
  data[3][1] = 0.0f;
  data[3][2] = 0.0f;
  data[3][3] = value;
}

Matrix4::Matrix4(f32 v00, f32 v01, f32 v02, f32 v03, //
                 f32 v10, f32 v11, f32 v12, f32 v13, //
                 f32 v20, f32 v21, f32 v22, f32 v23, f32 v30, f32 v31, f32 v32,
                 f32 v33) {
  data[0][0] = v00;
  data[0][1] = v01;
  data[0][2] = v02;
  data[0][3] = v03;

  data[1][0] = v10;
  data[1][1] = v11;
  data[1][2] = v12;
  data[1][3] = v13;

  data[2][0] = v20;
  data[2][1] = v21;
  data[2][2] = v22;
  data[2][3] = v23;

  data[3][0] = v30;
  data[3][1] = v31;
  data[3][2] = v32;
  data[3][3] = v33;
}

Matrix4::Matrix4(Matrix4 const &copy) {
  for (int r = 0; r < SIZE; r++)
    for (int c = 0; c < SIZE; c++)
      this->data[r][c] = copy.data[r][c];
}

void Matrix4::set(i32 row, i32 col, f32 value) { data[row][col] = value; }

f32 Matrix4::operator()(i32 row, i32 col) { return this->data[row][col]; }
f32 Matrix4::operator()(i32 row, i32 col) const { return this->data[row][col]; }

Matrix4 Matrix4::operator+(Matrix4 &right) {
  Matrix4 value(0.0f);
  for (int r = 0; r < SIZE; r++) {
    for (int c = 0; c < SIZE; c++) {
      value.set(r, c, (*this)(r, c) + right(r, c));
    }
  }
  return value;
}

Matrix4 Matrix4::operator-(Matrix4 &right) {
  Matrix4 value(0.0f);
  for (int r = 0; r < SIZE; r++) {
    for (int c = 0; c < SIZE; c++) {
      value.set(r, c, (*this)(r, c) - right(r, c));
    }
  }
  return value;
}

Matrix4 Matrix4::operator*(f32 scalar) {
  Matrix4 value((*this));

  for (int r = 0; r < SIZE; r++)
    for (int c = 0; c < SIZE; c++)
      value.data[r][c] *= scalar;

  return value;
}

Matrix4 Matrix4::operator*(Matrix4 &right) {
  Matrix4 value(0.0f);

  for (int r = 0; r < SIZE; r++)
    for (int c = 0; c < SIZE; c++)
      for (int i = 0; i < SIZE; i++)
        value.set(r, c, value(r, c) + (*this)(r, i) * right(i, c));

  return value;
}

Matrix4 &Matrix4::operator=(Matrix4 other) noexcept {
  for (int r = 0; r < SIZE; r++) {
    for (int c = 0; c < SIZE; c++) {
      this->set(r, c, other(r, c));
    }
  }
  return *this;
}

void Matrix4::transpose() {
  for (int r = 0; r < SIZE; r++) {
    for (int c = 0; c < SIZE; c++) {
      f32 temp = (*this)(r, c);
      this->set(r, c, (*this)(c, r));
      this->set(c, r, temp);
    }
  }
}

f32 Matrix4::determinate() {
  Matrix3 s1 = submatrix(0, 0, *this);
  Matrix3 s2 = submatrix(1, 0, *this);
  Matrix3 s3 = submatrix(2, 0, *this);
  Matrix3 s4 = submatrix(3, 0, *this);

  return (*this)(0, 0) * s1.determinate() - (*this)(1, 0) * s2.determinate() +
         (*this)(2, 0) * s3.determinate() - (*this)(3, 0) * s4.determinate();
}

void Matrix4::inverse() {
  Matrix4 inverted(0.0);

  inverted.data[0][0] = submatrix(0, 0, (*this)).determinate();
  inverted.data[0][1] = -submatrix(1, 0, (*this)).determinate();
  inverted.data[0][2] = submatrix(2, 0, (*this)).determinate();
  inverted.data[0][3] = -submatrix(3, 0, (*this)).determinate();

  inverted.data[1][0] = -submatrix(0, 1, (*this)).determinate();
  inverted.data[1][1] = submatrix(1, 1, (*this)).determinate();
  inverted.data[1][2] = -submatrix(2, 1, (*this)).determinate();
  inverted.data[1][3] = submatrix(3, 1, (*this)).determinate();

  inverted.data[2][0] = submatrix(0, 2, (*this)).determinate();
  inverted.data[2][1] = -submatrix(1, 2, (*this)).determinate();
  inverted.data[2][2] = submatrix(2, 2, (*this)).determinate();
  inverted.data[2][3] = -submatrix(2, 3, (*this)).determinate();

  inverted.data[3][0] = -submatrix(0, 3, (*this)).determinate();
  inverted.data[3][1] = submatrix(1, 3, (*this)).determinate();
  inverted.data[3][2] = -submatrix(2, 3, (*this)).determinate();
  inverted.data[3][3] = submatrix(3, 3, (*this)).determinate();

  (*this) = inverted * (1.0f / this->determinate());
}

Matrix3 submatrix(i32 row, i32 col, const Matrix4 &m) {
  Matrix3 value(0.0f);

  i32 indexRow = 0;
  i32 indexCol = 0;

  for (int r = 0; r < 4; r++)
    for (int c = 0; c < 4; c++)
      if (r != row && c != col) {
        value.set(indexRow, indexCol, m(r, c));
        indexCol = (indexCol + 1) % 3;
        if (indexCol == 0)
          indexRow = (indexRow + 1) % 3;
      }

  return value;
}

}; // namespace Math
