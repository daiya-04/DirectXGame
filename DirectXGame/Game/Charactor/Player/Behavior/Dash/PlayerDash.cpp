#include "PlayerDash.h"
#include "Player.h"
#include "Input.h"

#include <cassert>

void PlayerDash::Init() {

	assert(player_);

	player_->SetAnimation(Player::Action::kDash);
	player_->GetFollowCamera()->SetDashFlag(true);
	player_->SetSpeed(dashSpeed_);

	consumeStamina_ = 15.0f;

}

void PlayerDash::Update() {
	
	//攻撃
	if (DaiEngine::Input::GetInstance()->TriggerButton(DaiEngine::Input::Button::X)) {
		player_->GetFollowCamera()->SetDashFlag(false);
		player_->ChangeBehavior("Attack");
		return;
	}

	//回避
	if (DaiEngine::Input::GetInstance()->TriggerButton(DaiEngine::Input::Button::B) && player_->IsAvoid()) {
		player_->ChangeBehavior("Avoid");
		return;
	}

	if (DaiEngine::Input::GetInstance()->ReleaseButton(DaiEngine::Input::Button::A) || !DaiEngine::Input::GetInstance()->TiltLStick(DaiEngine::Input::Stick::All)) {
		player_->GetFollowCamera()->SetDashFlag(false);
		//スティック入力がされたままだったらJogStateへ
		if (DaiEngine::Input::GetInstance()->TiltLStick(DaiEngine::Input::Stick::All)) {
			player_->ChangeBehavior("Jog");
		}
		else {
			player_->ChangeBehavior("Idel");
		}
		return;
	}

	if (player_->GetStamina().GetStamina() <= 0.0f) {
		player_->ChangeBehavior("Jog");
	}

	//加速
	dashSpeed_ += absAccel_;
	dashSpeed_ = std::clamp(dashSpeed_, 0.0f, maxSpeed_);
	player_->SetSpeed(dashSpeed_);

	player_->Move();

	player_->GetStamina().ConsumeDash(consumeStamina_);

}
