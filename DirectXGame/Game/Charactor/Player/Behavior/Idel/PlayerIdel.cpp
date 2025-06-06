#include "PlayerIdel.h"
#include "Player.h"
#include "Input.h"

#include <cassert>

void PlayerIdel::Init() {

	assert(player_);

	player_->SetAnimation(Player::Action::kStanding);


}

void PlayerIdel::Update() {

	player_->StaminaHeal();

	//Lスティック入力がされたらJogStateへ
	if (DaiEngine::Input::GetInstance()->TiltLStick(DaiEngine::Input::Stick::All)) {
		//Aボタンが入力されていたらダッシュ
		if (DaiEngine::Input::GetInstance()->PushButton(DaiEngine::Input::Button::B) && player_->IsDash()) {
			player_->ChangeBehavior("Dash");
			return;
		}
		else {
			player_->ChangeBehavior("Jog");
			return;
		}
		
	}

	//攻撃
	if (player_->AttackCommand()) {
		return;
	}

	//回避
	if (DaiEngine::Input::GetInstance()->TriggerButton(DaiEngine::Input::Button::A) && player_->IsAvoid()) {
		player_->ChangeBehavior("Avoid");
	}

}
