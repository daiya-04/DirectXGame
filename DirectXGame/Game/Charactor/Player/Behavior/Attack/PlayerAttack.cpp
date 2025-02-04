#include "PlayerAttack.h"
#include "Player.h"
#include "Input.h"

#include <cassert>

const std::array<PlayerAttack::ComboAttack, PlayerAttack::comboNum_> PlayerAttack::kComboAttacks_ = {
	{
		{30,20,5},
		{30,20,5},
		{30,20,10}
	}
};

void PlayerAttack::Init() {

	assert(player_);

	player_->SetAnimation(Player::Action::Attack, false);
	player_->GetNowAnimation().TimeReset();
	player_->GetNowAnimation().SetAnimationSpeed(attackAnimeSpeed_);
	player_->GetNowSkelton().Update();

	if (player_->GetTarget()) {
		//攻撃の発射方向計算
		Vector3 direction = player_->GetTarget()->translation_ - player_->GetObj()->worldTransform_.translation_;
		//射程内ならターゲット方向に向く
		if (direction.Length() <= attackRange_) {
			player_->SetDirection(direction);
		}
	}

}

void PlayerAttack::Update() {

	//1コンボ目、2コンボ目だったら入力を受け付ける
	if (comboIndex_ < comboNum_ - 1) {
		if (DaiEngine::Input::GetInstance()->TriggerButton(DaiEngine::Input::Button::X)) {
			comboNext_ = true;
		}
	}

	//攻撃の総時間計算
	uint32_t totalAttackTime = kComboAttacks_[comboIndex_].attackTime_;
	totalAttackTime += kComboAttacks_[comboIndex_].chargeTime_;
	totalAttackTime += kComboAttacks_[comboIndex_].recoveryTime_;

	//今コンボのどのフェーズか
	if (count_ >= kComboAttacks_[comboIndex_].chargeTime_ + kComboAttacks_[comboIndex_].attackTime_) {
		inComboPhase_ = static_cast<uint32_t>(ComboPhase::Recovery);
	}
	else if (count_ >= kComboAttacks_[comboIndex_].chargeTime_) {
		inComboPhase_ = static_cast<uint32_t>(ComboPhase::Attack);
	}
	else {
		inComboPhase_ = static_cast<uint32_t>(ComboPhase::Charge);
	}

	//攻撃が終了したら
	if (count_ >= totalAttackTime) {
		//攻撃が続くか
		if (comboNext_) {
			comboNext_ = false;
			comboIndex_++;
			comboIndex_ = std::clamp(comboIndex_, static_cast<uint32_t>(ComboIndex::First), static_cast<uint32_t>(ComboIndex::Third));

			Init();
		}
		else {
			//続かないなら...
			//スティック入力がされたままだったらJogStateへ
			if (DaiEngine::Input::GetInstance()->TiltLStick(DaiEngine::Input::Stick::All)) {
				player_->ChangeBehavior("Jog");
			}
			else {
				player_->ChangeBehavior("Idel");
			}
			comboIndex_ = static_cast<uint32_t>(ComboIndex::First);
			return;
		}
	}

	if (count_ == kComboAttacks_[comboIndex_].chargeTime_) {
		player_->ShotMagicBall();
	}


	count_++;
}
