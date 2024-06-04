/**
 *
 */
#pragma once

#include "../util/defines.hpp"
#include "vector.hpp"

namespace Math {

#define ROW_COL_TO_INDEX(R, C, S) (S * R) + C

// =============================================================================
// Matrix 3x3
// =============================================================================
class Matrix3 {

  /*
   * Stored as Column-Major Order
   *
   * 0  3  6
   * 1  4  7
   * 2  5  8
   *
   * Functions calls will assume Row-Major Order
   */

public:
  f32 m[9]{};

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
};

// =============================================================================
// Matrix 4x4
// =============================================================================
class Matrix4 {
public:
  f32 m[16]{};

  Matrix4();
  ~Matrix4() = default;
  Matrix4(f32 value);
  Matrix4(f32 v00, f32 v01, f32 v02, f32 v03, //
          f32 v10, f32 v11, f32 v12, f32 v13, //
          f32 v20, f32 v21, f32 v22, f32 v23, //
          f32 v30, f32 v31, f32 v32, f32 v33);
  Matrix4(const Matrix4 &copy);
  Matrix4(Matrix4 &&copy) noexcept;
  Matrix4 &operator=(const Matrix4 &other);
  Matrix4 &operator=(Matrix4 &&other) noexcept;

  void set(i32 row, i32 col, f32 value);
  f32 operator()(i32 row, i32 col);
  f32 operator()(i32 row, i32 col) const;
  Matrix4 operator+(Matrix4 &right);
  Matrix4 operator-(Matrix4 &right);
  Matrix4 operator*(Matrix4 &right);
  Matrix4 operator*(f32 scalar);

  f32 determinate();
  void transpose();
  void inverse();

  void print();

private:
  static Matrix3 submatrix(i32 row, i32 col, const Matrix4 &m);
};

// ===================================================================================================================
// 3D Graphics
// ===================================================================================================================
Matrix4 perspectiveMatrix(f32 fov, f32 aspect, f32 zNear, f32 zFar);
Matrix4 viewMatrix(const Vector3 &cameraPosition, const Vector3 &cameraTarget, const Vector3 &up);
Matrix4 modelMatrix(const Vector3 &translation, const Vector3 &rotation, const Vector3 &scale);

Matrix4 scale(const Matrix4 &m, const Vector3 &scale);
Matrix4 translate(const Matrix4 &m, const Vector3 &translation);
Matrix4 rotate(const Matrix4 &m, const Vector3 &rotation);

// =============================================================================
// Quaternions
// =============================================================================
struct Quaternion {
  Quaternion() = delete;
  Quaternion(f32 angle, const Vector3 &axis);
  Quaternion(f32 w, f32 i, f32 j, f32 k);
  Quaternion(const Quaternion &other);
  Quaternion(Quaternion &&other) noexcept;
  Quaternion &operator=(const Quaternion &other);
  Quaternion &operator=(Quaternion &&other) noexcept;

  Quaternion operator*(const Quaternion &other);
  Quaternion operator*(f32 scalar);
  Quaternion operator+(const Quaternion &other);
  Quaternion conjugate();
  void normalize();
  f32 length();

  f32 w;
  Vector3 v;
};

} // namespace Math
