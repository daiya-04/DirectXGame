#include "BossDead.h"
#include "Boss.h"
#include "RandomEngine.h"

#include <cassert>


void BossDead::Init() {

	assert(boss_);

	boss_->GetNowAnimation().SetAnimationSpeed(0.0f);
	isVib_ = true;
	point_ = boss_->GetCenterPos();

}

void BossDead::Update() {

	if (isVib_) {
		if (vibCounter_ <= vibTime_) {
			randomVec_ = {
			RandomEngine::GetFloatRandom(-randomRange_,randomRange_),
			RandomEngine::GetFloatRandom(-randomRange_,randomRange_),
			RandomEngine::GetFloatRandom(-randomRange_,randomRange_),
			};

			boss_->GetObj()->worldTransform_.translation_ = point_ + randomVec_;
		}
		else {
			boss_->GetObj()->worldTransform_.translation_ = point_;
		}
		if (++vibCounter_ >= waitTime_) {
			boss_->DeadEffStart();
			boss_->GetObj()->SetVisible(false);
			isVib_ = false;
		}
	}
	else {
		if (++counter_ >= deadTime_) {
			boss_->FinishDeadMotion();
		}
	}
	
}
