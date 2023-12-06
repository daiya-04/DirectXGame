#pragma once
#include "Vec3.h"
#include "Matrix44.h"

class ViewProjection{
public:

	Vector3 translation_ = { 0.0f, 0.0f, -10.0f };
	Vector3 rotation_ = {};

	Matrix4x4 matView_;
	Matrix4x4 matProjection_;

	float fovAngleY = 45.0f * 3.141592654f / 180.0f;
	float aspectRatio = (float)16 / 9;
	float nearZ = 0.1f;
	float farZ = 1000.0f;

public:

	void Initialize();

	void UpdateMatrix();

	void UpdateViewMatrix();

	void UpdateProjectionMatrix();

};

