#pragma once
#include "IPlayerBehavior.h"
class PlayerJog : public IPlayerBehavior {
public:

	PlayerJog(Player* player) : player_(player) {}

	~PlayerJog() override {}

	void Init() override;

	void Update() override;

private:

	Player* player_ = nullptr;

	float speed_ = 0.3f;

};

