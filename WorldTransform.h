#pragma once
#include "Vec3.h"
#include "Matrix44.h"


class WorldTransform{
public:

	Vector3 scale_ = { 1.0f,1.0f,1.0f };
	
	Vector3 rotation_ = {};

	Vector3 translation_ = {};

	Matrix4x4 matWorld_;

	const WorldTransform* parent_ = nullptr;
	const WorldTransform* translationParent_ = nullptr;

public:
	void UpdateMatrix();

};

