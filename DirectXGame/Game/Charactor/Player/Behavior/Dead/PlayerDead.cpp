#include "PlayerDead.h"
#include "Player.h"

#include <cassert>


void PlayerDead::Init() {

	assert(player_);

	player_->SetAnimation(Player::Action::kDead, false);
	player_->SetDirection(Vector3(0.0f, 0.0f, 1.0f));

}

void PlayerDead::Update() {

	if (!player_->GetNowAnimation().IsPlaying()) {
		player_->FinishDeadMotion();
	}

	player_->DissolveUpdate();

}
