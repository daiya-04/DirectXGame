#include "PlayerAvoid.h"
#include "Player.h"
#include "Input.h"

#include <cassert>


void PlayerAvoid::Init() {

	assert(player_);

	player_->SetAnimation(Player::Action::kAvoid,false);
	player_->GetNowAnimation().SetAnimationSpeed(1.0f / 30.0f);

	if (DaiEngine::Input::GetInstance()->TiltLStick(DaiEngine::Input::Stick::All)) {
		//移動ベクトル
		Vector3 move{};
		move = DaiEngine::Input::GetInstance()->GetMoveXZ();
		move = (move / SHRT_MAX);
		move = TransformNormal(move, GetRotateYMatrix(player_->GetFollowCamera()->GetRotateMat()));
		player_->SetDirection(move.Normalize());
	}
	
	Vector3 zeroVector{};
	if (player_->GetDirection() == zeroVector) {
		direction_ = { 0.0f,0.0f,1.0f };
	}
	else {
		direction_ = player_->GetDirection();
	}

	player_->GetCollider()->ColliderOff();
	player_->GetStamina().ConsumeAvoid(consumeStamina_);

}

void PlayerAvoid::Update() {

	avoidSpeed_ += absAccel_;
	avoidSpeed_ = std::clamp(avoidSpeed_, 0.0f, maxSpeed_);

	player_->GetObj()->worldTransform_.translation_ += direction_.Normalize() * avoidSpeed_;

	if (!player_->GetNowAnimation().IsPlaying()) {
		if (DaiEngine::Input::GetInstance()->TiltLStick(DaiEngine::Input::Stick::All)) {
			if (DaiEngine::Input::GetInstance()->PushButton(DaiEngine::Input::Button::B)) {
				player_->ChangeBehavior("Dash");
			}else {
				player_->ChangeBehavior("Jog");
			}
		}else {
			player_->ChangeBehavior("Idel");
		}

		player_->GetCollider()->ColliderOn();

	}

}
