#pragma once
#include "WorldTransform.h"
#include "Camera.h"
#include "Object3d.h"
#include "Vec3.h"
#include <memory>

class Ground{
private:

	std::unique_ptr<Object3d> obj_;
	WorldTransform worldTransform_;

public:

	void Init(uint32_t modelHandle);

	void Update();

	void Draw(const Camera& camera);

};

