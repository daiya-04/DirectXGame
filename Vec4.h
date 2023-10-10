#pragma once

class Vector4 {
public:

	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 0.0f;

public:

	Vector4() {};
	Vector4(float x, float y, float z, float w) :x(x), y(y), z(z), w(w) {};
	Vector4(const Vector4& vector) :x(vector.x), y(vector.y), z(vector.z), w(vector.w) {};

};