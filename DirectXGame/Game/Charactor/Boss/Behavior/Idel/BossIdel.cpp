#include "BossIdel.h"
#include "Boss.h"
#include <cassert>

void BossIdle::Init() {

	assert(boss_);

	boss_->SetAnimation(Boss::Action::Standing);
	boss_->SetDirection(Vector3(0.0f, 0.0f, -1.0f));

	counter_ = 0;

}

void BossIdle::Update() {

	if (++counter_ >= coolTime_) {
		boss_->ChangeBehavior("Attack");
	}

}
