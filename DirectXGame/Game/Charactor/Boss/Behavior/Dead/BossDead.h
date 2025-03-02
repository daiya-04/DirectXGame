#pragma once
#include "IBossBehavior.h"
#include "Vec3.h"

class BossDead : public IBossBehavior {
public:

	BossDead(Boss* boss) : boss_(boss) {}

	~BossDead() override = default;

	void Init() override;

	void Update() override;

private:

	Boss* boss_ = nullptr;

	Vector3 randomVec_;
	float randomRange_ = 0.3f;
	Vector3 point_;

	uint32_t counter_ = 0;
	uint32_t deadTime_ = 70;

	uint32_t vibCounter_ = 0;
	uint32_t vibTime_ = 70;
	uint32_t waitTime_ = vibTime_ + 40;
	bool isVib_ = true;

};

