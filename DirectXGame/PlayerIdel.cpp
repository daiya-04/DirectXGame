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
	if (Input::GetInstance()->TiltLStick(Input::Stick::All)) {
		player_->ChangeBehavior("Jog");
	}

	//攻撃
	if (Input::GetInstance()->TriggerButton(Input::Button::X)) {
		player_->ChangeBehavior("Attack");
	}

	////ダッシュ
	//if (Input::GetInstance()->TriggerButton(Input::Button::RIGHT_SHOULDER)) {
	//	player_->ChangeBehavior("Dash");
	//}

}
