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

	timer_ = 0;

}

void PlayerKnockBack::Update() {

	if (!player_->GetNowAnimation().IsPlaying()) {
		//スティック入力がされたままだったらJogStateへ
		if (DaiEngine::Input::GetInstance()->TiltLStick(DaiEngine::Input::Stick::All)) {
			if (DaiEngine::Input::GetInstance()->PushButton(DaiEngine::Input::Button::A)) {
				player_->ChangeBehavior("Dash");
			}
			else {
				player_->ChangeBehavior("Jog");
			}
		}else {
			player_->ChangeBehavior("Idel");
		}
	}

	if (++timer_ >= avoidCoolTime_) {
		if (DaiEngine::Input::GetInstance()->TriggerButton(DaiEngine::Input::Button::B)) {
			player_->ChangeBehavior("Avoid");
		}
	}
	

	backSpeed_ += absAccel_;
	backSpeed_ = std::clamp(backSpeed_, 0.0f, maxSpeed_);

	player_->GetObj()->worldTransform_.translation_ += knockBackDict_ * backSpeed_;

}
