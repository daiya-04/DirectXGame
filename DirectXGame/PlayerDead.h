#pragma once
#include "IPlayerBehavior.h"

class PlayerDead : public IPlayerBehavior {
public:

	PlayerDead(Player* player) : player_(player) {}
	
	~PlayerDead() override {}

	void Init() override;

	void Update() override;

private:

	Player* player_ = nullptr;

};

