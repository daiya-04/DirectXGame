#pragma once
#include "IBossBehavior.h"

class BossIdel : public IBossBehavior {
public:

	BossIdel(Boss* boss) : boss_(boss) {}

	~BossIdel() override = default;

	void Init() override;

	void Update() override;

private:

	Boss* boss_ = nullptr;

	int32_t counter_ = 0;
	//攻撃の間隔
	int32_t coolTime_ = 60 * 2;

};

