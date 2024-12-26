#include "PlayerJog.h"
#include "Player.h"
#include "Input.h"

#include <cassert>

void PlayerJog::Init() {

	assert(player_);

	player_->SetAnimation(Player::Action::Jogging);

}

void PlayerJog::Update() {

	//スティックを話したらIdelStateへ
	if (Input::GetInstance()->ReleaseLStick(Input::Stick::All)) {
		player_->ChangeBehavior("Idel");
		return;
	}

	//攻撃
	if (Input::GetInstance()->TriggerButton(Input::Button::X)) {
		player_->ChangeBehavior("Attack");
		return;
	}

	////ダッシュ
	//if (Input::GetInstance()->TriggerButton(Input::Button::RIGHT_SHOULDER)) {
	//	player_->ChangeBehavior("Dash");
	//}

	//移動ベクトル
	Vector3 move{};

	move = Input::GetInstance()->GetMoveXZ();
	move = (move / SHRT_MAX) * speed_;

	move = TransformNormal(move, MakeRotateYMatrix(player_->GetFollowCamera()->GetCamera().rotation_.y));

	player_->GetObj()->worldTransform_.translation_ += move;
	player_->GetObj()->worldTransform_.translation_.y = 0.0f;

	player_->SetDirection(move.Normalize());

}
