#pragma once
#include "IBossBehavior.h"
#include "Vec3.h"


class BossAppear : public IBossBehavior {
public:

	BossAppear(Boss* boss) : boss_(boss) {}

	~BossAppear() override = default;

	void Init() override;

	void Update() override;

private:

	Boss* boss_ = nullptr;

	Vector3 startPos_{};
	Vector3 endPos_{};
	const float speed_ = 0.005f;
	float param_ = 0.0f;
	float endPosHeight_ = 3.0f;

};

