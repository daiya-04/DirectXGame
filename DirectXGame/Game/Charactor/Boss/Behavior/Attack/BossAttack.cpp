#include "BossAttack.h"
#include "Boss.h"

#include <cassert>
#include "RandomEngine.h"

uint32_t BossAttack::attackCount_ = 0;

BossAttack::BossAttack(Boss* boss) {

	assert(boss);

	boss_ = boss;

	elementBall_ = boss_->GetAttackType<ElementBallManager>();
	groundFlare_ = boss_->GetAttackType<GroundFlareManager>();
	icicle_ = boss_->GetAttackType<IcicleManager>();
	plasmaShot_ = boss_->GetAttackType<PlasmaShotManager>();

}

void BossAttack::Init() {

	assert(boss_);

	attackCount_++;

	//攻撃の設定
	if (boss_->GetAttackType() == Boss::AttackType::kElementBall) {
		//データのセットと開始
		elementBall_->SetAttackData(boss_->GetObj()->GetWorldPos().GetXZ());
		elementBall_->AttackStart();
		//次の攻撃
		boss_->SetAttackType(Boss::AttackType::kGroundFlare);

	}else if (boss_->GetAttackType() == Boss::AttackType::kGroundFlare) {
		groundFlare_->AttackStart();
		//次の攻撃
		boss_->SetAttackType(Boss::AttackType::kIcicle);

	}else if (boss_->GetAttackType() == Boss::AttackType::kIcicle) {
		boss_->LookAtTarget();
		//データのセットと開始
		icicle_->SetAttackData(boss_->GetCenterPos(), boss_->GetDirection());
		icicle_->AttackStart();
		//次の攻撃
		boss_->SetAttackType(Boss::AttackType::kPlasmaShot);

	}else if (boss_->GetAttackType() == Boss::AttackType::kPlasmaShot) {
		//データのセットと開始
		boss_->LookAtTarget();
		Vector3 offset = { 0.0f,0.0f,2.5f };
		offset = Transform(offset, boss_->GetRotateMat());

		plasmaShot_->SetAttackData(boss_->GetCenterPos() + offset);
		plasmaShot_->AttackStart();

		//次の攻撃
		boss_->SetAttackType(Boss::AttackType::kElementBall);
	}

	//boss_->SetAnimation(Boss::Action::AttackSet,false);

}

void BossAttack::Update() {

	//攻撃のセット、攻撃のモーションが終わったら立ちアニメーションにする
	if ((boss_->GetActionIndex() == Boss::Action::Attack || boss_->GetActionIndex() == Boss::Action::AttackSet) && !boss_->GetNowAnimation().IsPlaying()) {
		//boss_->SetAnimation(Boss::Action::Standing);
	}
	//攻撃が始まったら攻撃のアニメーションに
	if (groundFlare_->FireStartFlag() || elementBall_->ShotStart()) {
		//boss_->SetAnimation(Boss::Action::Attack, false);
	}
	//攻撃が終わったら通常行動に
	if (groundFlare_->AttackFinish() || icicle_->AttackFinish() || plasmaShot_->AttackFinish() || elementBall_->AttackFinish()) {

		if (attackCount_ >= 2) {

			uint32_t threshold = static_cast<uint32_t>(RandomEngine::GetIntRandom(2, 3));

			if (attackCount_ >= threshold) {
				boss_->ChangeBehavior("Move");
				attackCount_ = 0;
			}
			else {
				boss_->ChangeBehavior("Idle");
			}

		}
		else {
			boss_->ChangeBehavior("Idle");
		}

		
	}

}
