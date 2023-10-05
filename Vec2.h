#pragma once
#include <cmath>
#include <algorithm>

class Vector2 {
public:
    float x = 0.0f;
    float y = 0.0f;
public:
    inline Vector2(){}
	inline Vector2(float x,float y) : x(x), y(y){}
	inline Vector2(const Vector2& vector) : x(vector.x), y(vector.y){}

	inline Vector2 operator-() const { return {-x,-y}; }

	inline Vector2& operator+=(const Vector2& vector){
		x += vector.x;
		y += vector.y;
		return *this;
	}

	inline Vector2& operator-=(const Vector2& vector){
		x -= vector.x;
		y -= vector.y;
		return *this;
	}

	inline Vector2& operator*=(const float scalar){
		x *= scalar;
		y *= scalar;
		return *this;
	}

	inline Vector2& operator/=(const float scalar){
		x /= scalar;
		y /= scalar;
		return *this;
	}

	friend inline Vector2 operator+(const Vector2& v1,const Vector2& v2){
		return { v1.x + v2.x, v1.y + v2.y };
	}

	friend inline Vector2 operator-(const Vector2& v1,const Vector2& v2){
		return { v1.x - v2.x, v1.y - v2.y };
	}

	friend inline Vector2 operator*(float scalar,const Vector2& vector){
		return { scalar * vector.x, scalar * vector.y };
	}

	friend inline Vector2 operator*(const Vector2& vector, float scalar){
		return scalar * vector;
	}

	friend inline Vector2 operator/(const Vector2& vector, float scalar){
		return { vector.x / scalar, vector.y / scalar };
	}

	friend inline bool operator==(const Vector2& v1,const Vector2& v2){
		return { v1.x == v2.x && v1.y == v2.y};
	}

	friend inline bool operator!=(const Vector2& v1,const Vector2& v2){
		return !(v1 == v2);
	}

	friend inline float Dot(const Vector2& v1,const Vector2& v2){
		return v1.x * v2.x + v1.y * v2.y;
	}

	friend inline float Cross(const Vector2& v1,const Vector2& v2){
		return v1.x * v2.y - v1.y * v2.x;
	}

	inline float Length() const {
		return std::sqrtf(x * x + y * y);
	}

	inline Vector2 Normalize() const {
		return *this / Length();
	}

	inline Vector2 Normal() const {
		return { -y, x };
	}

	inline Vector2 NegNormal() const {
		return { y, -x };
	}

	friend inline Vector2 Lerp(float t, const Vector2& start, const Vector2& end){
		t = std::clamp(t, 0.0f, 1.0f);
		return { (1.0f - t) * start + t * end };
	}

	friend inline Vector2 Bezier(const Vector2& p0, const Vector2& p1, const Vector2& p2, float t){
		Vector2 p0p1 = Lerp(t, p0, p1);
		Vector2 p1p2 = Lerp(t, p1, p2);
		Vector2 p = Lerp(t, p0p1, p1p2);

		return p;
	}
};