#pragma once
#include "Vec3.h"
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

};

inline  Quaternion IdentityQuaternion() { return { 0.0f,0.0f,0.0f,1.0f }; }