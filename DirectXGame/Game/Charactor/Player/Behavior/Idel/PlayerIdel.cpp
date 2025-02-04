#include "PlayerIdel.h"
#include "Player.h"
#include "Input.h"

#include <cassert>

void PlayerIdel::Init() {

	assert(player_);

	player_->SetAnimation(Player::Action::Standing);


}

void PlayerIdel::Update() {
	//Lスティック入力がされたらJogStateへ
	if (DaiEngine::Input::GetInstance()->TiltLStick(DaiEngine::Input::Stick::All)) {
		player_->ChangeBehavior("Jog");
	}

	//攻撃
	if (DaiEngine::Input::GetInstance()->TriggerButton(DaiEngine::Input::Button::X)) {
		player_->ChangeBehavior("Attack");
	}

	//ダッシュ
	if (DaiEngine::Input::GetInstance()->TriggerButton(DaiEngine::Input::Button::A)) {
		player_->ChangeBehavior("Dash");
	}

}
