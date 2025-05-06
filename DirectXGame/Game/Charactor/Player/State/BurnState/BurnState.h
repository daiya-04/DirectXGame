#pragma once
#include "IPlayerState.h"

class BurnState : public IPlayerState {
public:

	BurnState(Player* player) : player_(player) {}

	~BurnState() override;

	void Init() override;

	void Update() override;

	void OnCollision() override;

private:

	Player* player_ = nullptr;

	uint32_t effCoolTime_ = 60 * 2;
	uint32_t damageCoolTime_ = 20;
	uint32_t effTimeCounter_ = 0;
	uint32_t damageTimeCounter_ = 0;

	bool isDamage_ = false;

};

