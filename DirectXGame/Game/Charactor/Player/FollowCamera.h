#pragma once
#include "Camera.h"
#include "WorldTransform.h"
#include "Vec3.h"

class FollowCamera{
private:

	Camera camera_;
	const WorldTransform* target_ = nullptr;

	//追従対象の残像座標
	Vector3 interTarget_{};

	float delayParam_ = 0.0f;
	uint32_t delayTime_ = 15;

public:

	void Init();

	void Update();

	void Reset();

	Vector3 OffsetCalc() const;

	void SetTarget(const WorldTransform* target);

	Camera& GetCamera() { return camera_; }

};

