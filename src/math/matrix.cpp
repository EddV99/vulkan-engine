#include "matrix.hpp"
#include "../util/defines.hpp"
#include "vector.hpp"
#include <cmath>
#include <iostream>

namespace Math {

// =============================================================================
// Matrix 3x3
// =============================================================================
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

Matrix3::Matrix3()
    : m{1.0f, 0.0f, 0.0f, //
        0.0f, 1.0f, 0.0f, //
        0.0f, 0.0f, 1.0f} {}

Matrix3::Matrix3(f32 value)
    : m{value, 0.0f,  0.0f, //
        0.0f,  value, 0.0f, //
        0.0f,  0.0f,  value} {}

Matrix3::Matrix3(f32 v00, f32 v01, f32 v02, //
                 f32 v10, f32 v11, f32 v12, //
                 f32 v20, f32 v21, f32 v22)
    : m{v00, v10, v20, //
        v01, v11, v21, //
        v02, v12, v22} {}

Matrix3::Matrix3(Matrix3 const &copy) {
  for (int r = 0; r < 9; r++)
    this->m[r] = copy.m[r];
}

void Matrix3::set(i32 row, i32 col, f32 value) { m[ROW_COL_TO_INDEX(row, col, 3)] = value; }

f32 Matrix3::operator()(i32 row, i32 col) { return this->m[ROW_COL_TO_INDEX(row, col, 3)]; }
f32 Matrix3::operator()(i32 row, i32 col) const { return this->m[ROW_COL_TO_INDEX(row, col, 3)]; }

Matrix3 Matrix3::operator+(Matrix3 &right) {
  Matrix3 value(0.0f);
  for (int r = 0; r < 3; r++) {
    for (int c = 0; c < 3; c++) {
      value.set(r, c, (*this)(r, c) + right(r, c));
    }
  }
  return value;
}

Matrix3 Matrix3::operator-(Matrix3 &right) {
  Matrix3 value(0.0f);
  for (int r = 0; r < 3; r++) {
    for (int c = 0; c < 3; c++) {
      value.set(r, c, (*this)(r, c) - right(r, c));
    }
  }
  return value;
}

Matrix3 Matrix3::operator*(f32 scalar) {
  Matrix3 value((*this));

  for (int r = 0; r < 3; r++)
    for (int c = 0; c < 3; c++)
      value.set(r, c, value(r, c) * scalar);

  return value;
}

Matrix3 Matrix3::operator*(Matrix3 &right) {
  Matrix3 value(0.0f);

  for (int r = 0; r < 3; r++)
    for (int c = 0; c < 3; c++)
      for (int i = 0; i < 3; i++)
        value.set(r, c, value(r, c) + (*this)(r, i) * right(i, c));

  return value;
}

Matrix3 &Matrix3::operator=(Matrix3 other) noexcept {
  for (int r = 0; r < 3; r++) {
    for (int c = 0; c < 3; c++) {
      this->set(r, c, other(r, c));
    }
  }
  return *this;
}

void Matrix3::transpose() {
  Matrix3 t;

  for (int r = 0; r < 3; r++)
    for (int c = 0; c < 3; c++)
      t.set(r, c, (*this)(c, r));

  (*this) = t;
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

// =============================================================================
// Matrix 4x4
// =============================================================================
void Matrix4::print() {
  for (int r = 0; r < 4; r++) {
    for (int c = 0; c < 4; c++) {
      std::cout << (*this)(r, c) << " ";
    }
    std::cout << "\n";
  }
  std::cout << "\n";
}

Matrix4::Matrix4()
    : m{1.0f, 0.0f, 0.0f, 0.0f, //
        0.0f, 1.0f, 0.0f, 0.0f, //
        0.0f, 0.0f, 1.0f, 0.0f, //
        0.0f, 0.0f, 0.0f, 1.0f} {}

Matrix4::Matrix4(f32 value)
    : m{value, 0.0f,  0.0f,  0.0f, //
        0.0f,  value, 0.0f,  0.0f, //
        0.0f,  0.0f,  value, 0.0f, //
        0.0f,  0.0f,  0.0f,  value} {}

Matrix4::Matrix4(f32 v00, f32 v01, f32 v02, f32 v03, //
                 f32 v10, f32 v11, f32 v12, f32 v13, //
                 f32 v20, f32 v21, f32 v22, f32 v23, //
                 f32 v30, f32 v31, f32 v32, f32 v33)
    : m{v00, v10, v20, v30, //
        v01, v11, v21, v31, //
        v02, v12, v22, v32, //
        v03, v13, v23, v33} {}

Matrix4::Matrix4(const Matrix4 &other) {
  for (int r = 0; r < 16; r++)
    this->m[r] = other.m[r];
}
Matrix4::Matrix4(Matrix4 &&other) noexcept {
  for (int r = 0; r < 16; r++) {
    this->m[r] = other.m[r];
    other.m[r] = 0;
  }
}

Matrix4 &Matrix4::operator=(const Matrix4 &other) {
  if (this == &other)
    return *this;

  for (int r = 0; r < 16; r++)
    this->m[r] = other.m[r];

  return *this;
}

Matrix4 &Matrix4::operator=(Matrix4 &&other) noexcept {
  if (this == &other)
    return *this;

  for (int r = 0; r < 16; r++) {
    this->m[r] = other.m[r];
    other.m[r] = 0;
  }

  return *this;
}

Matrix3 Matrix4::toMatrix3x3() {
  return Matrix3((*this)(0, 0), (*this)(0, 1), (*this)(0, 2), //
                 (*this)(1, 0), (*this)(1, 1), (*this)(1, 2), //
                 (*this)(2, 0), (*this)(2, 1), (*this)(2, 2));
}

void Matrix4::set(i32 row, i32 col, f32 value) { m[ROW_COL_TO_INDEX(row, col, 4)] = value; }

f32 Matrix4::operator()(i32 row, i32 col) { return m[ROW_COL_TO_INDEX(row, col, 4)]; }
f32 Matrix4::operator()(i32 row, i32 col) const { return m[ROW_COL_TO_INDEX(row, col, 4)]; }

Matrix4 Matrix4::operator+(Matrix4 &right) {
  Matrix4 value(0.0f);
  for (int x = 0; x < 16; x++)
    value.m[x] = this->m[x] + right.m[x];
  return value;
}

Matrix4 Matrix4::operator-(Matrix4 &right) {
  Matrix4 value(0.0f);
  for (int x = 0; x < 16; x++)
    value.m[x] = this->m[x] - right.m[x];
  return value;
}

Matrix4 Matrix4::operator*(f32 scalar) {
  Matrix4 value(0.0f);

  for (int x = 0; x < 16; x++)
    value.m[x] = this->m[x] * scalar;

  return value;
}

Matrix4 Matrix4::operator*(Matrix4 &right) {
  Matrix4 value(0.0f);

  for (int r = 0; r < 4; r++)
    for (int c = 0; c < 4; c++)
      for (int i = 0; i < 4; i++)
        value.set(r, c, value(r, c) + (*this)(r, i) * right(i, c));

  return value;
}

void Matrix4::transpose() {
  for (int r = 0; r < 4; r++) {
    for (int c = 0; c < 4; c++) {
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

Matrix3 Matrix4::submatrix(i32 row, i32 col, const Matrix4 &m) {
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

// =============================================================================
// Quaternions
// =============================================================================
Quaternion::Quaternion(f32 w, f32 i, f32 j, f32 k) : w(w), v{i, j, k} {}

Quaternion::Quaternion(f32 angle, const Vector3 &axis) {
  angle = TO_RADIANS(angle);
  f32 sinHalf = sinf(angle * 0.5);
  f32 cosHalf = cosf(angle * 0.5);

  v.x = axis.x * sinHalf;
  v.y = axis.y * sinHalf;
  v.z = axis.z * sinHalf;
  w = cosHalf;
}
Quaternion::Quaternion(const Quaternion &other) : w(other.w), v(other.v) {}

Quaternion::Quaternion(Quaternion &&other) noexcept : w(other.w), v(other.v) {}

void Quaternion::print() { std::cout << "(" << w << "{" << v.x << " " << v.y << " " << v.z << "})\n"; }

Quaternion &Quaternion::operator=(Quaternion &&other) noexcept {
  if (this == &other)
    return *this;

  this->v = other.v;
  this->w = other.w;

  return *this;
}

Quaternion &Quaternion::operator=(const Quaternion &other) {
  if (this == &other)
    return *this;

  this->v = other.v;
  this->w = other.w;

  return *this;
}

Quaternion Quaternion::operator*(const Quaternion &other) {
  f32 x0 = w * other.w - v.dot(other.v);
  Math::Vector3 x = (other.v * w) + (v * other.w) + v.cross(other.v);
  return Quaternion{x0, x.x, x.y, x.z};
}
Quaternion Quaternion::operator*(f32 scalar) {
  return Quaternion(w * scalar, v.x * scalar, v.y * scalar, v.z * scalar);
}
Quaternion Quaternion::operator+(const Quaternion &other) {
  return Quaternion(w + other.w, v.x + other.v.x, v.y + other.v.y, v.z + other.v.z);
}

void Quaternion::rotate(Vector3 r) {
  f32 x0 = std::cos(TO_RADIANS(r.x) / 2.0f);
  f32 x1 = std::sin(TO_RADIANS(r.x) / 2.0f);

  f32 y0 = std::cos(TO_RADIANS(r.y) / 2.0f);
  f32 y1 = std::sin(TO_RADIANS(r.y) / 2.0f);

  f32 z0 = std::cos(TO_RADIANS(r.z) / 2.0f);
  f32 z1 = std::sin(TO_RADIANS(r.z) / 2.0f);

  f32 q0 = x0 * y0 * z0 + x1 * y1 * z1;
  f32 q1 = x1 * y0 * z0 - x0 * y1 * z1;
  f32 q2 = x0 * y1 * z0 + x1 * y0 * z1;
  f32 q3 = x0 * y0 * z1 - x1 * y1 * z0;

  this->w = q0;
  this->v = {q1, q2, q3};
}

f32 Quaternion::length() { return std::sqrt(w * w + v.x * v.x + v.y * v.y + v.z * v.z); }

Quaternion Quaternion::conjugate() { return Quaternion{w, Vector3(v * -1.0f)}; }

void Quaternion::normalize() {
  f32 length = 1.0f / this->length();
  this->w *= length;
  this->v.x *= length;
  this->v.y *= length;
  this->v.z *= length;
}

Matrix4 Quaternion::toRotationMatrix() {
  this->normalize();
  /* return Matrix4(2 * (w * w + v.x * v.x) - 1, 2 * (v.x * v.y - w * v.z), 2 * (v.x * v.z + w * v.y), 0, // */
  /*                2 * (v.x * v.y + w * v.z), 2 * (w * w + v.y * v.y) - 1, 2 * (v.y * v.z - w * v.x), 0, // */
  /*                2 * (v.x * v.z - w * v.y), 2 * (v.y * v.z + w * v.x), 2 * (w * w + v.z * v.z) - 1, 0, // */
  /*                0, 0, 0, 1); */

  f32 ww = w * w;
  f32 xx = v.x * v.x;
  f32 yy = v.y * v.y;
  f32 zz = v.z * v.z;

  f32 x = v.x;
  f32 y = v.y;
  f32 z = v.z;

  return Matrix4(ww + xx - yy - zz, 2 * x * y - 2 * w * z, 2 * x * z + 2 * w * y, 0, //
                 2 * x * y + 2 * w * z, ww - xx + yy - zz, 2 * y * z - 2 * w * x, 0, //
                 2 * x * z - 2 * w * y, 2 * y * z + 2 * w * x, ww - xx - yy + zz, 0, //
                 0, 0, 0, 1);
}
}; // namespace Math
