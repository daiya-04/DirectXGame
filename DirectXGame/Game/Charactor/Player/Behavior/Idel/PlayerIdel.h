#pragma once
#include "IPlayerBehavior.h"


class PlayerIdel : public IPlayerBehavior {
public:

	PlayerIdel(Player* player) : player_(player) {}

	~PlayerIdel() override = default;

	void Init() override;

	void Update() override;

private:

	Player* player_ = nullptr;

};

