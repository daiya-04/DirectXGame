#pragma once
#include "IBossBehavior.h"

class BossIdle : public IBossBehavior {
public:

	BossIdle(Boss* boss) : boss_(boss) {}

	~BossIdle() override = default;

	void Init() override;

	void Update() override;

private:

	Boss* boss_ = nullptr;

	int32_t counter_ = 0;
	//攻撃の間隔
	int32_t coolTime_ = 60 * 1;

};

