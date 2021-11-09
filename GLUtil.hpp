#include "tgaimage.h";
#include <math.h>;
#include <algorithm>
#include <cmath>

#define DEG2RAD 0.0174533f

#pragma once

float Min(float a0, float a1, float a2) {
	return min(a0, min(a1, a2));
}

float Max(float a0, float a1, float a2) {
	return max(a0, max(a1, a2));
}

float Lerp(Vector3& barCoo, float val0, float val1, float val2) {
	return (barCoo.x * val0) + (barCoo.y * val1) + (barCoo.z * val2);
}

Vector3 Lerp(Vector3& barCoo, Vector3& val0, Vector3& val1, Vector3& val2) {
	return (barCoo.x * val0) + (barCoo.y * val1) + (barCoo.z * val2);
}

Vector2 Lerp(Vector3& barCoo, Vector2& val0, Vector2& val1, Vector2& val2) {
	return (barCoo.x * val0) + (barCoo.y * val1) + (barCoo.z * val2);
}

Color32 Lerp(Vector3& barCoo, Color32& val0, Color32& val1, Color32& val2) {
	auto r = (uint8_t)(barCoo.x * val0.r() + barCoo.y * val1.r() + barCoo.z * val2.r());
	auto g = (uint8_t)(barCoo.x * val0.g() + barCoo.y * val1.g() + barCoo.z * val2.g());
	auto b = (uint8_t)(barCoo.x * val0.b() + barCoo.y * val1.b() + barCoo.z * val2.b());
	auto a = (uint8_t)(barCoo.x * val0.a() + barCoo.y * val1.a() + barCoo.z * val2.a());
	return Color32(r, g, b, a);
}


Vector4 HomogeneousCoordinate(Vector3& v, bool isPoint) {
	return Vector4(v.x, v.y, v.z, isPoint ? 1 : 0);
}

Vector3 HomogeneousDivide(Vector4& v) {
	if (v.w == 0) return (Vector3)v;
	return Vector3(v.x / v.w, v.y / v.w, v.z / v.w);
}


Vector3 TranslatePoint(Matrix4x4& mat, Vector3& point) {
	return HomogeneousDivide(mat * HomogeneousCoordinate(point, true));
}

Vector3 TranslateDir(Matrix4x4& mat, Vector3& dir) {
	return HomogeneousDivide(mat * HomogeneousCoordinate(dir, false)).Normalized();
}

Vector3 TranslateVector(Matrix4x4& mat, Vector3& vec) {
	return HomogeneousDivide(mat * HomogeneousCoordinate(vec, false));
}

// 坐标系变换
Matrix4x4 Translate(Vector3& o, Vector3& u, Vector3& v, Vector3& w) {
	auto um = u.Magnitude();
	auto vm = v.Magnitude();
	auto wm = w.Magnitude();

	float tdata[4][4]{
			{1, 0, 0, -o.x},
			{0, 1, 0, -o.y},
			{0, 0, 1, -o.z},
			{0, 0, 0, 1}
	};
	Matrix4x4 t(tdata);

	float sdata[4][4]{
		{1 / um, 0, 0, 0},
		{0, 1 / vm, 0, 0},
		{0, 0, 1 / wm, 0},
		{0, 0, 0, 1}
	};
	Matrix4x4 s(sdata);

	float rdata[4][4]{
		{u.x / um, u.y / um, u.z / um, 0},
		{v.x / vm, v.y / vm, v.z / vm, 0},
		{w.x / wm, w.y / wm, w.z / wm, 0},
		{0, 0, 0, 1}
	};
	Matrix4x4 r(rdata);

	return s * r * t;
}

Matrix4x4 AffineToCooSystem(Vector3& o, Vector3& u, Vector3& v, Vector3& w) {
	auto um = u.Magnitude();
	auto vm = v.Magnitude();
	auto wm = w.Magnitude();

	float tdata[4][4]{
		{1, 0, 0, -o.x},
		{0, 1, 0, -o.y},
		{0, 0, 1, -o.z},
		{0, 0, 0, 1}
	};
	Matrix4x4 t(tdata);

	float sdata[4][4]{
		{um, 0, 0, 0},
		{0, vm, 0, 0},
		{0, 0, wm, 0},
		{0, 0, 0, 1}
	};
	Matrix4x4 s(sdata);

	float rdata[4][4]{
		{u.x / um, v.x / vm, w.x / wm, 0},
		{u.y / um, v.y / vm, w.y / wm, 0},
		{u.z / um, v.z / vm, w.z / wm, 0},
		{0, 0, 0, 1}
	};
	Matrix4x4 r(rdata);

	return s * r * t;
}

Matrix4x4 TRS(Vector3& pos, Vector3& rot, Vector3& scale) {
	float tdata[4][4] = {
		{1, 0, 0, pos.x},
		{0, 1, 0, pos.y},
		{0, 0, 1, pos.z},
		{0, 0, 0, 1}
		};
	Matrix4x4 t(tdata);

	float sdata[4][4] = {
		{scale.x, 0, 0, 0},
		{0, scale.y, 0, 0},
		{0, 0, scale.z, 0},
		{0, 0, 0, 1}
	};
	Matrix4x4 s(sdata);

	auto radianX = rot.x * DEG2RAD;
	auto radianY = rot.y * DEG2RAD;
	auto radianZ = rot.z * DEG2RAD;

	float rx[4][4]{
		{1, 0, 0, 0},
		{0, cos(radianX), -sin(radianX), 0},
		{0, sin(radianX), cos(radianX), 0},
		{0, 0, 0, 1}
		};
	float ry[4][4]{
		{cos(radianY), 0, sin(radianY), 0},
		{0, 1, 0, 0},
		{-sin(radianY), 0, cos(radianY), 0},
		{0, 0, 0, 1}
		};
	float rz[4][4]{
		{cos(radianZ), -sin(radianZ), 0, 0},
		{sin(radianZ), cos(radianZ), 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1}
		};
	auto r = Matrix4x4(ry) * Matrix4x4(rx) * Matrix4x4(rz);
	// var r = rz * ry * rx;
	return t * r * s;
}

#pragma region MVP Viewport

Matrix4x4 ModelMat(Vector3& pos, Vector3& rot, Vector3& scale) {
	return TRS(pos, rot, scale);
}

Matrix4x4 ViewMat(Vector3& camPos, Vector3& camDir, Vector3& camUp) {
	return Translate(camPos, Vector3::Cross(camUp, -camDir), camUp, -camDir);
}

Matrix4x4 Perspect(float fovy, float aspect, float near, float far, float& top, float& right) {
	top = near * tan(fovy / 2 * DEG2RAD);
	right = top * aspect;
	float data[4][4]{
		{near,0,0,0},
		{0,near,0,0},
		{0,0,near + far,-near * far},
		{0,0,1,0}
	};
	return Matrix4x4(data);
}
Matrix4x4 Ortho(float top, float right, float near, float far) {
	auto left = -right;
	auto bottom = -top;
	float data[4][4]{
		{2 / (right - left),0,0,-(right + left) / (right - left)},
		{0,2 / (top - bottom),0,-(top + bottom) / (top - bottom)},
		{0,0,2 / (near - far),-(near + far) / (near - far)},
		{0,0,0,1},
	};
	return Matrix4x4(data);
}
Matrix4x4 PerspectProjMat(float fovy, float aspect, float near, float far) {
	float top, right;
	auto perspectMat = Perspect(fovy, aspect, near, far, top, right);
	auto ortho = Ortho(top, right, near, far);
	return ortho * perspectMat;
}

Matrix4x4 ViewportMat(float width, float height) {
	float data[4][4]{
		{width / 2,0,0,width / 2},
		{0,height / 2,0,height / 2},
		{0,0,1,0},
		{0,0,0,1},
	};
	return Matrix4x4(data);
}

#pragma endregion

// 包围盒
void getBoundingBox(Vector2& p0, Vector2& p1, Vector2& p2, int& xmin, int& xmax,
 int& ymin, int& ymax) {
	xmin = roundf(Min(p0.x, p1.x, p2.x));
	xmax = roundf(Max(p0.x, p1.x, p2.x));
	ymin = roundf(Min(p0.y, p1.y, p2.y));
	ymax = roundf(Max(p0.y, p1.y, p2.y));
}

float crossProductZ(Vector2& a, Vector2& b) {
	return a.x * b.y - b.x * a.y;
}

// 重心坐标
Vector3 barycentricCoordinate(Vector2& p, Vector2& a, Vector2& b, Vector2& c) {
	auto x = (-(p.x - b.x) * (c.y - b.y) + (p.y - b.y) * (c.x - b.x)) /
		(-(a.x - b.x) * (c.y - b.y) + (a.y - b.y) * (c.x - b.x));
	auto y = (-(p.x - c.x) * (a.y - c.y) + (p.y - c.y) * (a.x - c.x)) /
		(-(b.x - c.x) * (a.y - c.y) + (b.y - c.y) * (a.x - c.x));
	auto z = 1 - x - y;
	return Vector3(x, y, z);
}

bool isPointInsideTriangle(Vector2& p, Vector2& p0, Vector2& p1, Vector2& p2) {
	auto bar = barycentricCoordinate(p, p0, p1, p2);
	return bar.x >= 0 && bar.y >= 0 && bar.z >= 0;
}
