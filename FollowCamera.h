#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Vec3.h"
#include "Input.h"

class LockOn;

class FollowCamera{
private:

	ViewProjection viewProjection_;
	const WorldTransform* target_ = nullptr;
	//追従対象の残像座標
	Vector3 interTarget_ = {};

	float num = 0.0f;

	uint32_t delayTime_ = 0;

	const LockOn* lockOn_ = nullptr;

public:

	void Initialize();

	void Update();

	void Reset();

	void ApplyGlobalVariables();

	Vector3 OffsetCalc() const;

	void SetTarget(const WorldTransform* target);
	void SetLockOn(const LockOn* lockOn) { lockOn_ = lockOn; }

	ViewProjection& GetViewProjection() { return viewProjection_; }

};

