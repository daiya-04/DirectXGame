#include "PlayerKnockBack.h"
#include "Player.h"
#include "Input.h"

#include <cassert>


void PlayerKnockBack::Init() {

	assert(player_);

	player_->SetAnimation(Player::Action::KnockBack,false);
	player_->GetNowAnimation().SetAnimationSpeed(attackAnimeSpeed_);
	player_->SetDirection(player_->GetKnockBackBaseDict().Normalize());

	knockBackDict_ = -player_->GetKnockBackBaseDict().Normalize();

}

void PlayerKnockBack::Update() {

	if (!player_->GetNowAnimation().IsPlaying()) {
		//スティック入力がされたままだったらJogStateへ
		if (DaiEngine::Input::GetInstance()->TiltLStick(DaiEngine::Input::Stick::All)) {
			player_->ChangeBehavior("Jog");
		}else {
			player_->ChangeBehavior("Idel");
		}
	}

	backSpeed_ += absAccel_;
	backSpeed_ = std::clamp(backSpeed_, 0.0f, maxSpeed_);

	player_->GetObj()->worldTransform_.translation_ += knockBackDict_ * backSpeed_;

}
