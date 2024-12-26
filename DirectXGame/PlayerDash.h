#pragma once
#include "IPlayerBehavior.h"
#include "Vec3.h"

class PlayerDash : public IPlayerBehavior {
public:

	PlayerDash(Player* player) : player_(player) {}

	~PlayerDash() override {}

	void Init() override;

	void Update() override;

private:

	Player* player_ = nullptr;

	//ダッシュ中の現在の時間
	uint32_t count_ = 0;
	//ダッシュの方向
	Vector3 dashDirection_ = {};
	//ダッシュの時間
	uint32_t dashTime_ = 10;

};

