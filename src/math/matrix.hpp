/**
 *
 */
#pragma once

#include "../util/defines.hpp"

namespace Math {

class Matrix3 {
public:
  Matrix3();
  Matrix3(f32 value);
  Matrix3(Matrix3 const &copy);
  Matrix3(f32 v00, f32 v01, f32 v02, //
          f32 v10, f32 v11, f32 v12, //
          f32 v20, f32 v21, f32 v22);

  void set(i32 row, i32 col, f32 value);
  f32 operator()(i32 row, i32 col);
  f32 operator()(i32 row, i32 col) const;
  Matrix3 operator+(Matrix3 &right);
  Matrix3 operator-(Matrix3 &right);
  Matrix3 operator*(Matrix3 &right);
  Matrix3 operator*(f32 scalar);
  Matrix3 &operator=(Matrix3 other) noexcept;

  f32 determinate();
  void transpose();
  void inverse();

  void print();

private:
  const i32 SIZE = 3;
  f32 data[3][3]{};
};

class Matrix4 {
public:
  Matrix4();
  Matrix4(f32 value);
  Matrix4(Matrix4 const &copy);
  Matrix4(f32 v00, f32 v01, f32 v02, f32 v03, //
          f32 v10, f32 v11, f32 v12, f32 v13, //
          f32 v20, f32 v21, f32 v22, f32 v23, //
          f32 v30, f32 v31, f32 v32, f32 v33);

  void set(i32 row, i32 col, f32 value);
  f32 operator()(i32 row, i32 col);
  f32 operator()(i32 row, i32 col) const;
  Matrix4 operator+(Matrix4 &right);
  Matrix4 operator-(Matrix4 &right);
  Matrix4 operator*(Matrix4 &right);
  Matrix4 operator*(f32 scalar);
  Matrix4 &operator=(Matrix4 other) noexcept;

  f32 determinate();
  void transpose();
  void inverse();

  void print();

private:
  const i32 SIZE = 4;
  f32 data[4][4]{};
};

Matrix3 submatrix(i32 row, i32 col, const Matrix4 &m);

} // namespace Math
