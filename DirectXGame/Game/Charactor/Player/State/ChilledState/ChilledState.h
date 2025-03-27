#pragma once
#include "IPlayerState.h"

class ChilledState : public IPlayerState {
public:

	ChilledState(Player* player) : player_(player) {}

	~ChilledState() override;

	void Init() override;

	void Update() override;

	void Exit() override;


private:

	Player* player_ = nullptr;

	float chilledRate_ = 0.6f;
	float normalRate_ = 1.0f;

	bool isSpeedDown_ = false;

	uint32_t effCoolTime_ = 60 * 2;
	uint32_t counter_ = 0;

};

