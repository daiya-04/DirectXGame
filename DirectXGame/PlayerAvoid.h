#pragma once
#include "IPlayerBehavior.h"
#include "Vec3.h"

class PlayerAvoid : public IPlayerBehavior {
public:

	PlayerAvoid(Player* player) : player_(player) {}

	~PlayerAvoid() override = default;

	void Init() override;

	void Update() override;

private:

	Player* player_ = nullptr;
	//回避の方向
	Vector3 direction_ = {};
	//加速量(スカラー)
	float absAccel_ = 0.01f;
	//回避の速さ
	float avoidSpeed_ = 0.0f;
	//速さの最大値
	float maxSpeed_ = 0.25f;

};

