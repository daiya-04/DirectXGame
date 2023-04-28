#pragma once

const int kColumnWidht = 60;
const int kRowHeight = 20;

typedef struct {
	float x, y, z;
}Vector3;

Vector3 Add(const Vector3& v1, const Vector3& v2);
Vector3 Sub(const Vector3& v1, const Vector3& v2);
Vector3 Mul(float scalar, const Vector3& v);
float Dot(const Vector3& v1, const Vector3& v2);
float Length(const Vector3& v);
Vector3 Normalize(const Vector3& v);

//void VectorScreenPrintf(int x, int y, const Vec3& v, const char* label);