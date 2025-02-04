#include "PlayerDash.h"
#include "Player.h"
#include "Input.h"

#include <cassert>

void PlayerDash::Init() {

	assert(player_);

	player_->SetAnimation(Player::Action::Accel);

	Vector3 zeroVector{};
	if (player_->GetDirection() == zeroVector) {
		dashDirection_ = { 0.0f,0.0f,1.0f };
	}else {
		dashDirection_ = player_->GetDirection();
	}
	player_->GetFollowCamera()->Reset();

	player_->GetCollider()->ColliderOff();
	

}

void PlayerDash::Update() {
	//加速
	dashSpeed_ += absAccel_;
	dashSpeed_ = std::clamp(dashSpeed_, 0.0f, maxSpeed_);

	player_->GetObj()->worldTransform_.translation_ += dashDirection_.Normalize() * dashSpeed_;

	if (++count_ >= dashTime_) {
		//スティック入力がされたままだったらJogStateへ
		if (DaiEngine::Input::GetInstance()->TiltLStick(DaiEngine::Input::Stick::All)) {
			player_->ChangeBehavior("Jog");
		}else {
			player_->ChangeBehavior("Idel");
		}
		player_->GetCollider()->ColliderOn();
	}

}
