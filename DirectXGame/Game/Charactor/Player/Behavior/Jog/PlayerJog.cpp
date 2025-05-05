#include "PlayerJog.h"
#include "Player.h"
#include "Input.h"

#include <cassert>

void PlayerJog::Init() {

	assert(player_);

	player_->SetAnimation(Player::Action::kJogging);
	player_->GetFollowCamera()->SetDashFlag(false);
	player_->SetSpeed(speed_);

}

void PlayerJog::Update() {

	player_->StaminaHeal();

	//スティックを離したらIdelStateへ
	if (DaiEngine::Input::GetInstance()->ReleaseLStick(DaiEngine::Input::Stick::All)) {
		player_->ChangeBehavior("Idel");
		return;
	}

	//攻撃
	if (player_->AttackCommand()) {
		return;
	}

	//ダッシュ
	if (DaiEngine::Input::GetInstance()->TriggerButton(DaiEngine::Input::Button::A) && player_->IsDash()) {
		player_->ChangeBehavior("Dash");
	}
	
	//回避
	if (DaiEngine::Input::GetInstance()->TriggerButton(DaiEngine::Input::Button::B) && player_->IsAvoid()) {
		player_->ChangeBehavior("Avoid");
	}

	player_->Move();

}
