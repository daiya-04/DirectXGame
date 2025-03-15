#pragma once
#include "IPlayerBehavior.h"
#include "Vec3.h"

class PlayerDash : public IPlayerBehavior {
public:

	PlayerDash(Player* player) : player_(player) {}

	~PlayerDash() override = default;

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
	//加速量(スカラー)
	float absAccel_ = 0.02f;
	//ダッシュの速さ
	float dashSpeed_ = 0.15f;
	//速さの最大値
	float maxSpeed_ = 0.2f;

};

