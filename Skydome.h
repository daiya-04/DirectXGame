#pragma once
#include <memory>
#include "Vec3.h"
#include "Object3d.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

class Skydome{
private:

	std::unique_ptr<Object3d> object_;
	WorldTransform worldTransform_;

public:

	void Initialize(uint32_t modelHandle);

	void Update();

	void Draw(const ViewProjection& viewProjection);

};

