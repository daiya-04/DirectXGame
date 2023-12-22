#pragma once
#include "Camera.h"
#include "WorldTransform.h"
#include "Vec3.h"

class FollowCamera{
private:

	Camera camera_;
	const WorldTransform* target_ = nullptr;

public:

	void Init();

	void Update();

	void SetTarget(const WorldTransform* target) { target_ = target; }

	Camera& GetCamera() { return camera_; }

};

