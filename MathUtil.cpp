#include "MathUtil.h"

#pragma region Vector2Int
Vector2Int::Vector2Int(int x, int y) :x(x), y(y)
{
}

Vector2Int::operator Vector2()
{
	return Vector2(x, y);
}

#pragma endregion

#pragma region Vector2

Vector2::Vector2(float x, float y) :x(x), y(y)
{
}

float Vector2::Magnitude()
{
	return sqrt(x * x + y * y);
}

Vector2 Vector2::Normalized()
{
	auto m = Magnitude();
	return Vector2(x / m, y / m);
}

Vector2 operator+(Vector2 a, Vector2 b)
{
	return Vector2(a.x + b.x, a.y + b.y);
}

Vector2 operator*(float a, Vector2 b)
{
	return Vector2(a * b.x, a * b.y);
}

Vector2 operator-(Vector2 a)
{
	return Vector2(-a.x, -a.y);
}

Vector2 operator-(Vector2 a, Vector2 b)
{
	return Vector2(a.x - b.x, a.y - b.y);
}

float Vector2::Dot(Vector2 a, Vector2 b)
{
	return a.x * b.x + a.y * b.y;
}
#pragma endregion

#pragma region Vector3

Vector3::Vector3(float x, float y, float z) :x(x), y(y), z(z)
{
}

float Vector3::Magnitude()
{
	return sqrt(x * x + y * y + z * z);
}

Vector3 Vector3::Normalized()
{
	auto m = Magnitude();
	return Vector3(x / m, y / m, z / m);
}

Vector3::operator Vector2()
{
	return Vector2(x, y);
}

Vector3 operator/(Vector3 a, float b)
{
	return Vector3(a.x / b, a.y / b, a.z / b);
}

Vector3 operator-(Vector3 a)
{
	return Vector3(-a.x, -a.y, -a.z);
}

Vector3 operator-(Vector3 a, Vector3 b)
{
	return Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vector3 operator+(Vector3 a, Vector3 b)
{
	return Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vector3 operator*(float a, Vector3 b)
{
	return Vector3(a * b.x, a * b.y, a * b.z);
}

Vector3 operator*(Vector3 b, float a)
{
	return Vector3(a * b.x, a * b.y, a * b.z);
}

Vector3 Vector3::Zero()
{
	return Vector3(0, 0, 0);
}

Vector3 Vector3::Back()
{
	return Vector3(0, 0, -1);
}

Vector3 Vector3::Forward()
{
	return Vector3(0, 0, 1);
}

float Vector3::Dot(Vector3 a, Vector3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector3 Vector3::Cross(Vector3 lhs, Vector3 rhs)
{
	return Vector3(lhs.y * rhs.z - rhs.y * lhs.z,
		-(lhs.x * rhs.z - rhs.x * lhs.z),
		lhs.x * rhs.y - rhs.x * lhs.y);
}
#pragma endregion

#pragma region Vector4

Vector4::Vector4()
{
}

Vector4::Vector4(float x, float y, float z, float w) :x(x), y(y), z(z), w(w)
{
}

float& Vector4::operator[](int i)
{
	switch (i) {
	case 0: return x;
	case 1: return y;
	case 2: return z;
	case 3: return w;
	}
	throw "out of range!";
}

Vector4::operator Vector3()
{
	return Vector3(x, y, z);
}

float Vector4::Dot(Vector4 a, Vector4 b)
{
	float sum = 0.f;
	for (int i = 0; i < 4; i++) {
		sum += a[i] * b[i];
	}
	return sum;
}

#pragma endregion

#pragma region Matrix

Matrix4x4 operator*(Matrix4x4& a, Matrix4x4& b)
{
	Matrix4x4 ret;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			float sum = 0;
			for (int k = 0; k < 4; k++)
			{
				sum += a.data[i][k] * b.data[k][j];
			}
			ret.data[i][j] = sum;
		}
	}
	return ret;
}

Matrix4x4 operator/(Matrix4x4& a, float b)
{
	Matrix4x4 ret;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			ret.data[i][j] = a.data[i][j] / b;
		}
	}
	return ret;
}

Vector4 operator*(Matrix4x4& lhs, Vector4& rhs)
{
	Vector4 ret;
	for (int i = 0; i < 4; i++) {
		ret[i] = Vector4::Dot(lhs.Row(i), rhs);
	}
	return ret;
}

float Matrix4x4::Det3x3(float m[][3]) {
	return
		m[0][0] * m[1][1] * m[2][2] + m[0][1] * m[1][2] * m[2][0] + m[0][2] * m[1][0] * m[2][1]
		- m[0][2] * m[1][1] * m[2][0] - m[0][1] * m[1][0] * m[2][2] - m[0][0] * m[1][2] * m[2][1];
}
float Matrix4x4::Cofactor(Matrix4x4& a, int i, int j)
{
	int sign = (i + j) % 2 == 0 ? 1 : -1;
	float m[3][3];
	for (int im = 0; im < 3; im++)
	{
		for (int jm = 0; jm < 3; jm++)
		{
			int ia = im;
			if (ia >= i) ia++;
			int ja = jm;
			if (ja >= j) ja++;
			m[im][jm] = a.data[ia][ja];
		}
	}
	return sign * Det3x3(m);
}

float Matrix4x4::Det(Matrix4x4& a)
{
	float ret = 0;
	for (int i = 0; i < 4; i++)
	{
		ret += a.data[0][i] * Cofactor(a, 0, i);
	}
	return ret;
}

Matrix4x4 Matrix4x4::Adjugate(Matrix4x4& a)
{
	Matrix4x4 ret;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			ret.data[i][j] = Cofactor(a, i, j);
		}
	}
	return Transpose(ret);
}

Matrix4x4 Matrix4x4::Invert(Matrix4x4& a)
{
	return Adjugate(a) / Det(a);
}

Matrix4x4 Matrix4x4::Transpose(Matrix4x4& a)
{
	Matrix4x4 ret;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			ret.data[i][j] = a.data[j][i];
		}
	}
	return ret;
}

Matrix4x4::Matrix4x4(const float data[4][4]) {
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			this->data[i][j] = data[i][j];
		}
	}
}

Vector4 Matrix4x4::Row(int i)
{
	return Vector4(data[i][0], data[i][1], data[i][2], data[i][3]);
}

Matrix4x4 Matrix4x4::Inverse()
{
	return Matrix4x4::Invert(*this);
}

Matrix4x4 Matrix4x4::Transpose()
{
	return Transpose(*this);
}

Matrix4x4::Matrix4x4()
{
}

#pragma endregion


