#pragma once
#include "Vec3.h"
#include "Object3d.h"

class Ground{
private:

	Object3d* model_ = nullptr;
	Vector3 position_{};
	Vector3 scale_ = {};

public:

	void Initialize(Object3d* model);

	void Update();

	void Draw();

};

