#include "BossDead.h"
#include "Boss.h"

#include <cassert>


void BossDead::Init() {

	assert(boss_);

	boss_->SetAnimation(Boss::Action::Dead, false);

}

void BossDead::Update() {

	if (!boss_->GetNowAnimation().IsPlaying()) {
		boss_->FinishDeadMotion();
	}

	boss_->DissolveUpdate();
	
}
