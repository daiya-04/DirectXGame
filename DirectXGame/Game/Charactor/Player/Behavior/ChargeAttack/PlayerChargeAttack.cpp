#include "PlayerChargeAttack.h"
#include "Player.h"
#include "Input.h"

#include <cassert>

PlayerChargeAttack::~PlayerChargeAttack() {
	//チャージエフェクト終了
	player_->EndChargeEff();
}

void PlayerChargeAttack::Init() {

	assert(player_);

	//モーション開始
	player_->SetAnimation(Player::Action::kCharge);

	//向き矯正
	if (player_->GetTarget()) {
		//攻撃の発射方向計算
		Vector3 direction = player_->GetTarget()->GetWorldPos().GetXZ() - player_->GetObj()->GetWorldPos().GetXZ();
		//射程内ならターゲット方向に向く
		if (direction.Length() <= player_->GetAttackRange()) {
			player_->SetDirection(direction);
		}
	}

	//チャージエフェクトの開始
	player_->StartChargeEff();
	player_->ChargeEffPosUpdate();

}

void PlayerChargeAttack::Update() {

	count_++;
	count_ = std::clamp(count_, 0u, chargeTime_);

	if (count_ < chargeTime_) {
		//チャージキャンセル
		if (DaiEngine::Input::GetInstance()->ReleaseButton(DaiEngine::Input::Button::X)) {
			player_->DecideMoveBehavior();
		}
		//チャージキャンセルして回避へ
		if (DaiEngine::Input::GetInstance()->TriggerButton(DaiEngine::Input::Button::B) && player_->IsAvoid()) {
			player_->ChangeBehavior("Avoid");
		}

	}
	//チャージ完了
	if (count_ >= chargeTime_) {
		//チャージ攻撃発射
		player_->ShotMagicCannon();
		

		player_->DecideMoveBehavior();
	}

}
