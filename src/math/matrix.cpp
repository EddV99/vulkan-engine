#include "matrix.hpp"
#include <iostream>

namespace Math {

void Matrix3::print() {
  std::cout << this->m[0] << " ";
  std::cout << this->m[3] << " ";
  std::cout << this->m[6] << " ";
  std::cout << "\n";

  std::cout << this->m[1] << " ";
  std::cout << this->m[4] << " ";
  std::cout << this->m[7] << " ";
  std::cout << "\n";

  std::cout << this->m[2] << " ";
  std::cout << this->m[5] << " ";
  std::cout << this->m[8] << " ";
  std::cout << "\n";
}

Matrix3::Matrix3() {
  m[0] = 1.0f;
  m[1] = 0.0f;
  m[2] = 0.0f;

  m[3] = 0.0f;
  m[4] = 1.0f;
  m[5] = 0.0f;

  m[6] = 0.0f;
  m[7] = 0.0f;
  m[8] = 1.0f;
}
Matrix3::Matrix3(f32 value) {
  m[0] = value;
  m[1] = 0.0f;
  m[2] = 0.0f;

  m[3] = 0.0f;
  m[4] = value;
  m[5] = 0.0f;

  m[6] = 0.0f;
  m[7] = 0.0f;
  m[8] = value;
}

Matrix3::Matrix3(Matrix3 const &copy) {
  for (int r = 0; r < SIZE; r++)
    this->m[r] = copy.m[r];
}

Matrix3::Matrix3(f32 v00, f32 v01, f32 v02, //
                 f32 v10, f32 v11, f32 v12, //
                 f32 v20, f32 v21, f32 v22) {
  m[0] = v00;
  m[1] = v10;
  m[2] = v20;

  m[3] = v01;
  m[4] = v11;
  m[5] = v21;

  m[6] = v02;
  m[7] = v12;
  m[8] = v22;
}

void Matrix3::set(i32 row, i32 col, f32 value) { m[ROW_COL_TO_INDEX(row, col, SIZE_ROW)] = value; }

f32 Matrix3::operator()(i32 row, i32 col) { return this->m[ROW_COL_TO_INDEX(row, col, SIZE_ROW)]; }
f32 Matrix3::operator()(i32 row, i32 col) const { return this->m[ROW_COL_TO_INDEX(row, col, SIZE_ROW)]; }

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
      value.set(r, c, value(r, c) * scalar);

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
  return (*this)(0, 0) * ((*this)(1, 1) * (*this)(2, 2) - (*this)(2, 1) * (*this)(1, 2)) -
         (*this)(0, 1) * ((*this)(1, 0) * (*this)(2, 2) - (*this)(2, 0) * (*this)(1, 2)) +
         (*this)(0, 2) * ((*this)(1, 0) * (*this)(2, 1) - (*this)(2, 0) * (*this)(1, 1));
}

void Matrix3::inverse() {
  Matrix3 inverted(0.0);

  inverted.set(0, 0, (*this)(1, 1) * (*this)(2, 2) - (*this)(2, 1) * (*this)(1, 2));
  inverted.set(0, 1, (*this)(0, 2) * (*this)(2, 1) - (*this)(2, 2) * (*this)(0, 1));
  inverted.set(0, 2, (*this)(0, 1) * (*this)(1, 2) - (*this)(1, 1) * (*this)(0, 2));

  inverted.set(1, 0, (*this)(1, 2) * (*this)(2, 0) - (*this)(2, 2) * (*this)(1, 0));
  inverted.set(1, 1, (*this)(0, 0) * (*this)(2, 2) - (*this)(2, 0) * (*this)(0, 2));
  inverted.set(1, 2, (*this)(0, 2) * (*this)(1, 0) - (*this)(1, 2) * (*this)(0, 0));

  inverted.set(2, 0, (*this)(1, 0) * (*this)(2, 1) - (*this)(2, 0) * (*this)(1, 1));
  inverted.set(2, 1, (*this)(0, 1) * (*this)(2, 0) - (*this)(2, 1) * (*this)(0, 0));
  inverted.set(2, 2, (*this)(0, 0) * (*this)(1, 1) - (*this)(1, 0) * (*this)(0, 1));

  (*this) = inverted * (1.0f / this->determinate());
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

Matrix4::Matrix4() {
  m[0] = 1.0f;
  m[1] = 0.0f;
  m[2] = 0.0f;
  m[3] = 0.0f;

  m[4] = 0.0f;
  m[5] = 1.0f;
  m[6] = 0.0f;
  m[7] = 0.0f;

  m[8] = 0.0f;
  m[9] = 0.0f;
  m[10] = 1.0f;
  m[11] = 0.0f;

  m[12] = 0.0f;
  m[13] = 0.0f;
  m[14] = 0.0f;
  m[15] = 1.0f;
}
Matrix4::Matrix4(f32 value) {
  m[0] = value;
  m[1] = 0.0f;
  m[2] = 0.0f;
  m[3] = 0.0f;

  m[4] = 0.0f;
  m[5] = value;
  m[6] = 0.0f;
  m[7] = 0.0f;

  m[8] = 0.0f;
  m[9] = 0.0f;
  m[10] = value;
  m[11] = 0.0f;

  m[12] = 0.0f;
  m[13] = 0.0f;
  m[14] = 0.0f;
  m[15] = value;
}

Matrix4::Matrix4(f32 v00, f32 v01, f32 v02, f32 v03, //
                 f32 v10, f32 v11, f32 v12, f32 v13, //
                 f32 v20, f32 v21, f32 v22, f32 v23, //
                 f32 v30, f32 v31, f32 v32, f32 v33) {
  m[0] = v00;
  m[1] = v10;
  m[2] = v20;
  m[3] = v30;

  m[4] = v01;
  m[5] = v11;
  m[6] = v21;
  m[7] = v31;

  m[8] = v02;
  m[9] = v12;
  m[10] = v22;
  m[11] = v32;

  m[12] = v03;
  m[13] = v13;
  m[14] = v23;
  m[15] = v33;
}

Matrix4::Matrix4(Matrix4 const &copy) {
  for (int r = 0; r < SIZE; r++)
    this->m[r] = copy.m[r];
}

void Matrix4::set(i32 row, i32 col, f32 value) { m[ROW_COL_TO_INDEX(row, col, SIZE_ROW)] = value; }

f32 Matrix4::operator()(i32 row, i32 col) { return m[ROW_COL_TO_INDEX(row, col, SIZE_ROW)]; }
f32 Matrix4::operator()(i32 row, i32 col) const { return m[ROW_COL_TO_INDEX(row, col, SIZE_ROW)]; }

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
      value.set(r, c, value(r, c) * scalar);

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

  return (*this)(0, 0) * s1.determinate() - (*this)(1, 0) * s2.determinate() + (*this)(2, 0) * s3.determinate() -
         (*this)(3, 0) * s4.determinate();
}

void Matrix4::inverse() {
  Matrix4 inverted(0.0);

  inverted.set(0, 0, submatrix(0, 0, (*this)).determinate());
  inverted.set(0, 1, -submatrix(1, 0, (*this)).determinate());
  inverted.set(0, 2, submatrix(2, 0, (*this)).determinate());
  inverted.set(0, 3, -submatrix(3, 0, (*this)).determinate());

  inverted.set(1, 0, -submatrix(0, 1, (*this)).determinate());
  inverted.set(1, 1, submatrix(1, 1, (*this)).determinate());
  inverted.set(1, 2, -submatrix(2, 1, (*this)).determinate());
  inverted.set(1, 3, submatrix(3, 1, (*this)).determinate());

  inverted.set(2, 0, submatrix(0, 2, (*this)).determinate());
  inverted.set(2, 1, -submatrix(1, 2, (*this)).determinate());
  inverted.set(2, 2, submatrix(2, 2, (*this)).determinate());
  inverted.set(2, 3, -submatrix(2, 3, (*this)).determinate());

  inverted.set(3, 0, -submatrix(0, 3, (*this)).determinate());
  inverted.set(3, 1, submatrix(1, 3, (*this)).determinate());
  inverted.set(3, 2, -submatrix(2, 3, (*this)).determinate());
  inverted.set(3, 3, submatrix(3, 3, (*this)).determinate());

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
