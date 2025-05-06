#pragma once
#include "IPlayerState.h"
class NormalState : public IPlayerState {
public:

	NormalState(Player* player) : player_(player) {}

	~NormalState() override {}

	void Init() override;

	void Update() override;
	
	void OnCollision() override;

private:

	Player* player_ = nullptr;

};

