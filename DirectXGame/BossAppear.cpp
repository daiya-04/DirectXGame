#include "BossAppear.h"
#include "Boss.h"
#include "Easing.h"

#include <cassert>


void BossAppear::Init() {

	assert(boss_);

	startPos_ = boss_->GetObj()->GetWorldPos();
	endPos_ = startPos_;
	endPos_.y = 0.0f;

	boss_->AppearEffectStart();

}

void BossAppear::Update() {
	//移動が終わったら通常行動に
	if (param_ >= 1.0f) {
		boss_->ChangeBehavior("Idel");
		boss_->AppearEffectEnd();
		return;
	}

	float T = Easing::easeInOutQuart(param_);
	boss_->GetObj()->worldTransform_.translation_ = Lerp(T, startPos_, endPos_);

	param_ += speed_;

}
