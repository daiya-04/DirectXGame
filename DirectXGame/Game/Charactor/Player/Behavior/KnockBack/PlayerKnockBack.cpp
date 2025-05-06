#include "PlayerKnockBack.h"
#include "Player.h"
#include "Input.h"

#include <cassert>


void PlayerKnockBack::Init() {

	assert(player_);

	player_->SetAnimation(Player::Action::kKnockBack,false);
	player_->GetNowAnimation().SetAnimationSpeed(attackAnimeSpeed_);
	player_->SetDirection(player_->GetKnockBackBaseDict().Normalize());

	knockBackDict_ = -player_->GetKnockBackBaseDict().Normalize();

	timer_ = 0;

}

void PlayerKnockBack::Update() {

	player_->StaminaHeal();

	if (!player_->GetNowAnimation().IsPlaying()) {
		//スティック入力がされたままだったらJogStateへ
		player_->DecideMoveBehavior();
	}

	if (++timer_ >= avoidCoolTime_) {
		if (DaiEngine::Input::GetInstance()->TriggerButton(DaiEngine::Input::Button::A) && player_->IsAvoid()) {
			player_->ChangeBehavior("Avoid");
		}
	}
	

	backSpeed_ += absAccel_;
	backSpeed_ = std::clamp(backSpeed_, 0.0f, maxSpeed_);

	player_->GetObj()->worldTransform_.translation_ += knockBackDict_ * backSpeed_;

}
