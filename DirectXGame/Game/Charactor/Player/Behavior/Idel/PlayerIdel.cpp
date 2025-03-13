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
		//Aボタンが入力されていたらダッシュ
		if (DaiEngine::Input::GetInstance()->PushButton(DaiEngine::Input::Button::A)) {
			player_->ChangeBehavior("Dash");
		}
		else {
			player_->ChangeBehavior("Jog");
		}
		
	}

	//攻撃
	if (DaiEngine::Input::GetInstance()->TriggerButton(DaiEngine::Input::Button::X)) {
		player_->ChangeBehavior("Attack");
	}

	//回避
	if (DaiEngine::Input::GetInstance()->TriggerButton(DaiEngine::Input::Button::B)) {
		player_->ChangeBehavior("Avoid");
	}

}
