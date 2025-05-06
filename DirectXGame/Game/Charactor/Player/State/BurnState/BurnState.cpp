#include "BurnState.h"
#include "Player.h"

#include <cassert>

BurnState::~BurnState() {
	player_->EndBurnEff();
}

void BurnState::Init() {

	assert(player_);

	if (!isDamage_) {
		player_->StartBurnEff();
		damageTimeCounter_ = damageCoolTime_;
		effTimeCounter_ = effCoolTime_;
		isDamage_ = true;
	}

}

void BurnState::Update() {

	if (!isDamage_) {
		if (--effTimeCounter_ <= 0) {
			player_->ChangeState("Normal");
		}
		
		return;
	}

	if (--damageTimeCounter_ <= 0) {

		player_->GetHP().TakeDamage();
		damageTimeCounter_ = damageCoolTime_;

		if (player_->GetHP().GetHP() <= 0) {
			player_->Dead();
		}
		
	}

	isDamage_ = false;
}

void BurnState::OnCollision() {

	isDamage_ = true;

}
