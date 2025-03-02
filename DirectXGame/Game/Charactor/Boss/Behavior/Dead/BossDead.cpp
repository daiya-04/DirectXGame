#include "BossDead.h"
#include "Boss.h"

#include <cassert>


void BossDead::Init() {

	assert(boss_);

	boss_->GetNowAnimation().SetAnimationSpeed(0.0f);

}

void BossDead::Update() {



	if (++counter_ >= deadTime_) {
		boss_->FinishDeadMotion();
	}
	
}
