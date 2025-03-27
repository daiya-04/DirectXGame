#include "ChilledState.h"
#include "Player.h"

#include <cassert>


ChilledState::~ChilledState() {
	player_->EndChilledEff();
}

void ChilledState::Init() {

	assert(player_);

	player_->SetSpeedRate(chilledRate_);
	player_->StartChilledEff();
	isSpeedDown_ = true;

}

void ChilledState::Update() {

	if (!isSpeedDown_) {

		if (--counter_ <= 0) {

			player_->ChangeState("Normal");

		}

	}

}

void ChilledState::Exit() {

	isSpeedDown_ = false;
	counter_ = effCoolTime_;
	player_->SetSpeedRate(normalRate_);

}
