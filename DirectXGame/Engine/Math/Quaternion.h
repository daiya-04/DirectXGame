#pragma once
#include "Vec3.h"
#include "Matrix44.h"
#include "cmath"

class Quaternion {
public:

	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 0.0f;

private:
	inline Vector3 vector() const{ return { x,y,z }; }

public:

	Quaternion() {};
	Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {};
	Quaternion(const Quaternion& quaternion) : x(quaternion.x), y(quaternion.y), z(quaternion.z), w(quaternion.w) {};

	inline Quaternion operator-() const { return { -x,-y,-z,-w }; }

	friend inline Quaternion operator+(const Quaternion& q1, const Quaternion& q2) {
		return { q1.x + q2.x,q1.y + q2.y ,q1.z + q2.z ,q1.w + q2.w };
	}

	friend inline Quaternion operator-(const Quaternion& q1, const Quaternion& q2) {
		return { q1.x - q2.x,q1.y - q2.y ,q1.z - q2.z ,q1.w - q2.w };
	}


	friend inline Quaternion operator*(float scalar, const Quaternion& q) {
		return { q.x * scalar,q.y * scalar ,q.z * scalar ,q.w * scalar };
	}

	friend inline Quaternion operator*(const Quaternion& q, float scalar) {
		return scalar * q;
	}

	friend inline Quaternion operator*(const Quaternion& q, const Quaternion& r) {
		
		Vector3 i = Cross(q.vector(), r.vector()) + r.w * q.vector() + q.w * r.vector();
		float w = q.w * r.w - Dot(q.vector(), r.vector());
		
		return { i.x,i.y,i.z,w };
	}

	friend inline Quaternion operator/(const Quaternion& q, float scalar) {
		return { q.x / scalar,q.y / scalar ,q.z / scalar ,q.w / scalar };
	}

	

	inline Quaternion Conjugation() const{
		return { -x,-y,-z,w };
	}

	inline float Length() const {
		return sqrtf(powf(x, 2.0f) + powf(y, 2.0f) + powf(z, 2.0f) + powf(w, 2.0f));
	}

	inline Quaternion Normalize() const {
		return *this / Length();
	}

	inline Quaternion Inverse() const {
		return Conjugation() / powf(Length(), 2.0f);
	}

	inline Matrix4x4 MakeRotateMatrix() {
		Matrix4x4 result = MakeIdentity44();
		float xy = x * y;
		float wz = w * z;
		float xz = x * z;
		float wy = w * y;
		float yz = y * z;
		float wx = w * x;

		result.m[0][0] = powf(w, 2.0f) + powf(x, 2.0f) - powf(y, 2.0f) - powf(z, 2.0f);
		result.m[0][1] = 2.0f * (xy + wz);
		result.m[0][2] = 2.0f * (xz - wy);
		result.m[1][0] = 2.0f * (xy - wz);
		result.m[1][1] = powf(w, 2.0f) - powf(x, 2.0f) + powf(y, 2.0f) - powf(z, 2.0f);
		result.m[1][2] = 2.0f * (yz + wx);
		result.m[2][0] = 2.0f * (xz + wy);
		result.m[2][1] = 2.0f * (yz - wx);
		result.m[2][2] = powf(w, 2.0f) - powf(x, 2.0f) - powf(y, 2.0f) + powf(z, 2.0f);

		return result;
	}

};

inline Quaternion IdentityQuaternion() { return { 0.0f,0.0f,0.0f,1.0f }; }

inline Quaternion MakwRotateAxisAngleQuaternion(const Vector3& axis, float angle) {

	float w = std::cosf(angle / 2.0f);
	Vector3 v = axis * std::sinf(angle / 2.0f);

	return { v.x,v.y,v.z,w };
}

inline Vector3 RotateVector(const Vector3& v, const Quaternion& q) {

	Quaternion r = { v.x,v.y,v.z,0.0f };
	Quaternion rotated = q * r * q.Conjugation();

	return { rotated.x,rotated.y,rotated.z };
}

inline float Dot(const Quaternion& q1, const Quaternion& q2) {
	return { q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w };
}

inline Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t) {

	Quaternion q0_ = q0;
	Quaternion q1_ = q1;

	float dot = Dot(q0_, q1_);
	if (dot < 0) {
		q0_ = -q0_;
		dot = -dot;
	}

	float theta = std::acos(dot);

	float scale0 = sinf((1 - t) * theta) / sinf(theta);
	float scale1 = sinf(t * theta) / sinf(theta);

	return scale0 * q0 + scale1 * q1;
}
