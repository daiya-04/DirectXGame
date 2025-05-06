#pragma once
#include "IPlayerBehavior.h"


class PlayerChargeAttack : public IPlayerBehavior {
public:

	PlayerChargeAttack(Player* player) : player_(player) {}

	~PlayerChargeAttack() override;

	void Init() override;

	void Update() override;

private:

	Player* player_ = nullptr;

	uint32_t count_ = 0;
	uint32_t chargeTime_ = static_cast<uint32_t>(60 * 1.2f);

};
