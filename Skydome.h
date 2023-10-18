#pragma once
#include "Vec3.h"
#include "Object3d.h"

class Skydome{
private:

	Object3d* model_ = nullptr;
	Vector3 position_ = {};

public:

	void Initialize(Object3d* model);

	void Update();

	void Draw();

};

