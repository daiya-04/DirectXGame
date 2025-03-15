#include "PlayerJog.h"
#include "Player.h"
#include "Input.h"

#include <cassert>

void PlayerJog::Init() {

	assert(player_);

	player_->SetAnimation(Player::Action::Jogging);
	player_->GetFollowCamera()->SetDashFlag(false);
	player_->SetSpeed(speed_);

}

void PlayerJog::Update() {

	//スティックを離したらIdelStateへ
	if (DaiEngine::Input::GetInstance()->ReleaseLStick(DaiEngine::Input::Stick::All)) {
		player_->ChangeBehavior("Idel");
		return;
	}

	//攻撃
	if (DaiEngine::Input::GetInstance()->TriggerButton(DaiEngine::Input::Button::X)) {
		player_->ChangeBehavior("Attack");
		return;
	}

	//ダッシュ
	if (DaiEngine::Input::GetInstance()->TriggerButton(DaiEngine::Input::Button::A)) {
		player_->ChangeBehavior("Dash");
		/*player_->SetAnimation(Player::Action::Dash);
		speed_ = dashSpeed_;
		player_->GetFollowCamera()->SetDashFlag(true);*/
	}
	////ダッシュ解除
	//if (DaiEngine::Input::GetInstance()->ReleaseButton(DaiEngine::Input::Button::A)) {
	//	player_->SetAnimation(Player::Action::Jogging);
	//	speed_ = baseSpeed_;
	//	player_->GetFollowCamera()->SetDashFlag(false);
	//}
	//回避
	if (DaiEngine::Input::GetInstance()->TriggerButton(DaiEngine::Input::Button::B)) {
		player_->ChangeBehavior("Avoid");
	}

	player_->Move();

}
