#include "BurnState.h"
#include "Player.h"

#include <cassert>

BurnState::~BurnState() {
	player_->EndBurnEff();
}

void BurnState::Init() {

	assert(player_);

	player_->StartBurnEff();

	counter = damageCoolTime_;
	isDamage_ = true;

}

void BurnState::Update() {

	if (--counter <= 0) {

		if (isDamage_) {

			player_->GetHP().TakeDamage();
			counter = damageCoolTime_;
			if (player_->GetHP().GetHP() <= 0) {
				player_->Dead();
			}

		}else {

			player_->ChangeState("Normal");

		}
		
	}

}

void BurnState::Exit() {

	isDamage_ = false;
	counter = effCoolTime_;

}
