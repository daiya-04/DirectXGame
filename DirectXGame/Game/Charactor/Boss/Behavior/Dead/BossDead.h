#pragma once
#include "IBossBehavior.h"

class BossDead : public IBossBehavior {
public:

	BossDead(Boss* boss) : boss_(boss) {}

	~BossDead() override = default;

	void Init() override;

	void Update() override;

private:

	Boss* boss_ = nullptr;

};

