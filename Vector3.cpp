#include"Vector3.h"
#include<math.h>
//#include<Novice.h>


Vector3 Add(const Vector3& v1, const Vector3& v2) {
    return { v1.x + v2.x,v1.y + v2.y,v1.z + v2.z };
}

Vector3 Sub(const Vector3& v1, const Vector3& v2) {
	return { v1.x - v2.x,v1.y - v2.y,v1.z - v2.z };
}

Vector3 Mul(float scalar, const Vector3& v) {
	return { scalar * v.x,scalar * v.y,scalar * v.z };
}

float Dot(const Vector3& v1, const Vector3& v2) {
	return { v1.x * v2.x + v1.y * v2.y + v1.z * v2.z };
}

float Length(const Vector3& v) {
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vector3 Normalize(const Vector3& v) {
	float len = Length(v);
	return { v.x / len,v.y / len,v.z / len };
}

//void VectorScreenPrintf(int x, int y, const Vec3& v, const char* label) {
//	Novice::ScreenPrintf(x, y, "%.02f", v.x);
//	Novice::ScreenPrintf(x + kColumnWidht, y, "%.02f", v.y);
//	Novice::ScreenPrintf(x + kColumnWidht * 2, y, "%.02f", v.z);
//	Novice::ScreenPrintf(x + kColumnWidht * 3, y, "%s", label);
//}