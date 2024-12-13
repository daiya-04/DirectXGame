#include "Boss.h"
#include <cmath>
#include <numbers>
#include "ModelManager.h"
#include "TextureManager.h"
#include "Easing.h"
#include "ModelManager.h"
#include "ShapesDraw.h"
#include "AnimationManager.h"
#include "ParticleManager.h"

#include "GroundFlare.h"

void Boss::Init(const std::vector<std::shared_ptr<Model>>& models) {

	//アクション設定
	actionIndex_ = Action::Standing;
	
	//モデル関連の初期化
	BaseCharactor::Init(models);

	///エフェクト初期化
	effect_ = ParticleManager::Load("BossEnter");
	for (auto& [group, particle] : effect_) {
		particle->particleData_.isLoop_ = false;
	}
	///

	//状態設定
	behaviorRequest_ = Behavior::kAppear;
	//攻撃設定
	attackType_ = AttackType::kElementBall;

	//HPの設定
	hp_ = maxHp_;
	hpBarSize_ = { 500.0f,10.0f };

	//UIの設定
	hpBar_.reset(Sprite::Create(TextureManager::Load("Boss_HP.png"), { 390.0f,40.0f }));
	hpBar_->SetAnchorpoint({ 0.0f,0.5f });
	hpBar_->SetSize(hpBarSize_);

}

void Boss::Update() {

	//状態が切り替わった時の初期化処理
	if (behaviorRequest_) {

		behavior_ = behaviorRequest_.value();

		behaviorInitTable_[behavior_]();

		behaviorRequest_ = std::nullopt;
	}

	//状態の更新
	behaviorUpdateTable_[behavior_]();

	BaseCharactor::Update();

	//エフェクト発生位置更新
	for (auto& [group, particle] : effect_) {
		particle->particleData_.emitter_.translate = obj_->GetWorldPos();
		particle->particleData_.emitter_.translate.y = 0.01f;
		particle->Update();
	}

}

void Boss::UpdateUI() {
	//現在のHPのパーセント計算
	float percent = static_cast<float>(hp_) / static_cast<float>(maxHp_);
	//HPのUIを割合に合わせる
	hpBar_->SetSize({ hpBarSize_.x * percent,hpBarSize_.y });
}

void Boss::Draw(const Camera& camera) {

	BaseCharactor::Draw(camera);

}

void Boss::DrawParticle(const Camera& camera) {
	for (auto& [group, particle] : effect_) {
		particle->Draw(camera);
	}
}

void Boss::DrawUI() {
	hpBar_->Draw();
}

void Boss::OnCollision() {
	hp_--;
	//HPが0になったら...
	if (hp_ <= 0) {
		isDead_ = true;
		behaviorRequest_ = Behavior::kDead;
	}
}

void Boss::RootInit() {

	workAttack_.param_ = 0;
	//立ちアニメーションに
	actionIndex_ = Action::Standing;
	animations_[actionIndex_].Start();
	direction_ = { 0.0f,0.0f,-1.0f };

	//向いてる方向から回転行列計算
	rotateMat_ = DirectionToDirection({ 0.0f,0.0f,1.0f }, direction_);

}

void Boss::RootUpdate() {
	//一定時間で攻撃
	if (++workAttack_.param_ > workAttack_.coolTime_) {
		behaviorRequest_ = Behavior::kAttack;
	}
	
}

void Boss::AttackInit() {

	//攻撃の設定
	if (attackType_ == AttackType::kElementBall) {
		//データのセットと開始
		elementBall_->SetAttackData(obj_->worldTransform_.translation_);
		elementBall_->AttackStart();
		
		//次の攻撃
		attackType_ = AttackType::kGroundFlare;
		
	}else if (attackType_ == AttackType::kGroundFlare) {
		groundFlare_->AttackStart();
		//次の攻撃
		attackType_ = AttackType::kIcicle;
	}
	else if (attackType_ == AttackType::kIcicle) {
		//データのセットと開始
		icicle_->SetAttackData(GetCenterPos(), Vector3(0.0f, 0.0f, -1.0f));
		icicle_->AttackStart();
		//次の攻撃
		attackType_ = AttackType::kPlasmaShot;
	}
	else if (attackType_ == AttackType::kPlasmaShot) {
		//データのセットと開始
		direction_ = (target_->translation_ - obj_->worldTransform_.translation_).Normalize();
		rotateMat_ = DirectionToDirection({ 0.0f,0.0f,1.0f }, direction_);
		Vector3 offset = { 0.0f,0.0f,2.0f };
		offset = Transform(offset, rotateMat_);

		plasmaShot_->SetAttackData(GetCenterPos() + offset);
		plasmaShot_->AttackStart();

		//次の攻撃
		attackType_ = AttackType::kElementBall;
	}

	actionIndex_ = Action::AttackSet;
	animations_[actionIndex_].Start(false);

}

void Boss::AttackUpdate() {
	//攻撃のセット、攻撃のモーションが終わったら立ちアニメーションにする
	if ((actionIndex_ == Action::Attack || actionIndex_ == Action::AttackSet) && !animations_[actionIndex_].IsPlaying()) {
		actionIndex_ = Action::Standing;
		animations_[actionIndex_].Start();
	}
	//攻撃が始まったら攻撃のアニメーションに
	if (groundFlare_->FireStartFlag() || elementBall_->ShotStart()) {
		actionIndex_ = Action::Attack;
	}
	//攻撃が終わったら通常行動に
	if (groundFlare_->AttackFinish() || icicle_->AttackFinish() || plasmaShot_->AttackFinish() || elementBall_->AttackFinish()) {
		behaviorRequest_ = Behavior::kRoot;
	}
	
}

void Boss::AppearInit() {

	workAppear_.startPos_ = obj_->GetWorldPos();
	workAppear_.endPos_ = obj_->GetWorldPos();
	workAppear_.endPos_.y = 0.0f;
	obj_->worldTransform_.translation_ = workAppear_.startPos_;
	workAppear_.param_ = 0.0f;

	//登場演出開始
	for (auto& [group, particle] : effect_) {
		particle->particleData_.isLoop_ = true;
	}

}

void Boss::AppearUpdate() {
	//移動が終わったら通常行動に
	if (workAppear_.param_ >= 1.0f) {
		behaviorRequest_ = Behavior::kRoot;
		for (auto& [group, particle] : effect_) {
			particle->particleData_.isLoop_ = false;
		}
		return;
	}

	//移動処理
	float T = Easing::easeInOutQuart(workAppear_.param_);
	obj_->worldTransform_.translation_ = Lerp(T, workAppear_.startPos_, workAppear_.endPos_);

	workAppear_.param_ += workAppear_.speed_;

}

void Boss::DeadInit() {

	actionIndex_ = Action::Dead;
	animations_[actionIndex_].Start(false);
	//ディゾルブ用パラメータ初期化
	obj_->threshold_ = 0.0f;
	
}

void Boss::DeadUpdate() {
	//アニメーション終了
	if (!animations_[actionIndex_].IsPlaying()) {
		isFinishDeadMotion_ = true;
	}
	//ディゾルブ用パラメータの計算
	obj_->threshold_ = animations_[actionIndex_].GetAnimationTime() / animations_[actionIndex_].GetDuration();
	obj_->threshold_ = std::clamp(obj_->threshold_, 0.0f, animations_[actionIndex_].GetDuration());

}
