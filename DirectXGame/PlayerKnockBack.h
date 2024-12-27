#pragma once
#include "IPlayerBehavior.h"
#include "Vec3.h"

class PlayerKnockBack : public IPlayerBehavior {
public:

	PlayerKnockBack(Player* player) : player_(player) {}

	~PlayerKnockBack() override {}

	void Init() override;

	void Update() override;

private:

	Player* player_ = nullptr;

	Vector3 knockBackDict_ = {};
	//速さの最大値
	float maxSpeed_ = 0.8f;
	//下がる速さ
	float backSpeed_ = maxSpeed_;
	//加速量(スカラー)
	float absAccel_ = -0.1f;

	//アニメーションのスピード
	float attackAnimeSpeed_ = 1.0f / 30.0f;

};

