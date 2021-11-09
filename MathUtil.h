#include <math.h>

#pragma once



struct Vector2 {
	float x, y;

	Vector2() {}
	Vector2(float x, float y);
	float Magnitude();
	Vector2 Normalized();
	static float Dot(Vector2 a, Vector2 b);
};
Vector2 operator +(Vector2 a, Vector2 b);
Vector2 operator *(float a, Vector2 b);
Vector2 operator -(Vector2 a);
Vector2 operator -(Vector2 a, Vector2 b);

struct Vector2Int
{
	int x, y;

	Vector2Int() {}
	Vector2Int(int x, int y);
	operator Vector2();
};

struct Vector3
{
	float x, y, z;
	Vector3() {}
	Vector3(float x, float y, float z);
	float Magnitude();
	Vector3 Normalized();
	operator Vector2();

	static Vector3 Zero();
	static Vector3 Back();
	static Vector3 Forward();
	static float Dot(Vector3 a, Vector3 b);
	static Vector3 Cross(Vector3 lhs, Vector3 rhs);

};
Vector3 operator /(Vector3 a, float b);
Vector3 operator -(Vector3 a);
Vector3 operator -(Vector3 a, Vector3 b);
Vector3 operator +(Vector3 a, Vector3 b);
Vector3 operator *(float a, Vector3 b);
Vector3 operator *(Vector3 b, float a);

struct Vector4
{
	float x, y, z, w;
	Vector4();
	Vector4(float x, float y, float z, float w);
	float& operator[](int i);
	operator Vector3();

	static float Dot(Vector4 a, Vector4 b);

};

struct Matrix4x4 {
	float data[4][4] = {};
	Matrix4x4();
	Matrix4x4(const float data[4][4]);
	Vector4 Row(int i);
	Matrix4x4 Inverse();
	Matrix4x4 Transpose();

	static float Cofactor(Matrix4x4& a, int i, int j);
	static float Det3x3(float m[][3]);
	static float Det(Matrix4x4& a);
	static Matrix4x4 Adjugate(Matrix4x4& a);
	static Matrix4x4 Invert(Matrix4x4& a);
	static Matrix4x4 Transpose(Matrix4x4& a);
};
Matrix4x4 operator*(Matrix4x4& a, Matrix4x4& b);
Vector4 operator*(Matrix4x4& m, Vector4& v);
Matrix4x4 operator/(Matrix4x4& a, float b);



