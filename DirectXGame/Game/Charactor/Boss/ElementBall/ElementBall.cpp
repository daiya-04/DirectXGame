#include "ElementBall.h"

#include "Easing.h"
#include "ShapesDraw.h"
#include "AnimationManager.h"
#include "ModelManager.h"
#include "TextureManager.h"
#include "ParticleManager.h"
#include "ColliderManager.h"

size_t ElementBall::heightAdjustmentIndex = 1;

ElementBall::~ElementBall() {
	DaiEngine::ColliderManager::GetInstance()->RemoveCollider(collider_.get());
}

void ElementBall::Init() {

	animeData_ = DaiEngine::ModelManager::LoadGLTF("ElementBall");
	animation_ = DaiEngine::AnimationManager::Load(animeData_->name_);
	animation_.SetAnimationSpeed(1.0f / 30.0f);

	worldTransform_.Init();

	collider_ = std::make_unique<DaiEngine::SphereCollider>();
	collider_->Init("BossAttack", worldTransform_, 1.3f);
	collider_->SetStayCallback([this](DaiEngine::Collider* other) { this->OnCollision(other); });
	DaiEngine::ColliderManager::GetInstance()->AddCollider(collider_.get());

	//エフェクト設定
	effect_ = ParticleManager::Load("FireBall");
	setEff_ = ParticleManager::Load("FireBallSet");

	for (auto& [name, particle] : effect_) {
		particle->particleData_.isLoop_ = false;
	}

	for (auto& [name, particle] : setEff_) {
		particle->particleData_.isLoop_ = false;
	}

	isLife_ = false;
	preIsLife_ = false;

}

void ElementBall::Update() {

	preIsLife_ = isLife_;
	//フェーズ切り替えの初期化
	if (phaseRequest_) {

		phase_ = phaseRequest_.value();

		phaseInitTable_[phase_]();

		phaseRequest_ = std::nullopt;
	}
	//フェーズ更新
	phaseUpdateTable_[phase_]();

	//行列更新
	worldTransform_.UpdateMatrix();

	if (animation_.IsPlaying()) {
		animation_.Play(animeData_->rootNode_);
		worldTransform_.matWorld_ = animation_.GetLocalMatrix() * worldTransform_.matWorld_;
	}

	for (auto& [name, particle] : effect_) {
		particle->particleData_.emitter_.translate = worldTransform_.GetWorldPos();
		particle->Update();
	}
	for (auto& [name, particle] : setEff_) {
		particle->Update();
	}

	collider_->Update();
}

void ElementBall::DrawParticle(const DaiEngine::Camera& camera) {
	for (auto& [name, particle] : effect_) {
		particle->Draw(camera);
	}
	for (auto& [name, particle] : setEff_) {
		particle->Draw(camera);
	}
}

void ElementBall::OnCollision([[maybe_unused]] DaiEngine::Collider* other) {
	if (other->GetTag() == "Player" || other->GetTag() == "Ground") {
		Dead();
	}
}

void ElementBall::Dead() {
	isLife_ = false;
	phaseRequest_ = Phase::kRoot;
	collider_->ColliderOff();

	burnScar_->EffectStart(worldTransform_.GetWorldPos());
	burnScar_->HeightAdjustment(0.0001f * heightAdjustmentIndex);
	heightAdjustmentIndex = (heightAdjustmentIndex % 4) + 1;
}

void ElementBall::AttackStart() {

	phaseRequest_ = Phase::kSet;
	isLife_ = true;

	for (auto& [name, particle] : effect_) {
		particle->particleData_.isLoop_ = true;
	}
	for (auto& [name, particle] : setEff_) {
		particle->particleData_.isLoop_ = true;
	}

}

void ElementBall::SetAttackData(const Vector3& startPos, uint32_t interval) {

	worldTransform_.translation_ = startPos;
	const uint32_t kFramePerSecond = 60;
	workCharge_.coolTime_ = kFramePerSecond * interval;
	for (auto& [name, particle] : effect_) {
		particle->particleData_.emitter_.translate = startPos;
	}
	for (auto& [name, particle] : setEff_) {
		particle->particleData_.emitter_.translate = startPos;
		particle->particleData_.emitter_.translate.y = 0.01f;
	}

}

void ElementBall::RootInit() {

	for (auto& [name, particle] : effect_) {
		particle->particleData_.isLoop_ = false;
	}
	animation_.TimeReset();

}

void ElementBall::RootUpdate() {



}

void ElementBall::SetInit() {
	
	animation_.Start(false);

}

void ElementBall::SetUpdate() {
	//セットアニメーションが終わったら溜めに
	if (!animation_.IsPlaying()) {
		phaseRequest_ = Phase::kCharge;
		animation_.End();
		//ローカル座標更新
		worldTransform_.translation_ = worldTransform_.GetWorldPos();

		for (auto& [name, particle] : setEff_) {
			particle->particleData_.isLoop_ = false;
		}
	}

}

void ElementBall::ChargeInit() {

	workCharge_.param_ = 0;

}

void ElementBall::ChargeUpdate() {
	//時間が経ったら発射
	if (++workCharge_.param_ >= workCharge_.coolTime_) {
		phaseRequest_ = Phase::kShot;
	}

}

void ElementBall::ShotInit() {

	workShot_.isTrack_ = true;
	collider_->ColliderOn();

}

void ElementBall::ShotUpdate() {
	//進む方向を計算
	Vector3 diff = *target_ - worldTransform_.translation_;
	float distance = diff.Length();
	const float kSpeed = 0.6f;
	//一定距離まで追尾
	if (distance < workShot_.trackingDist_) {
		workShot_.isTrack_ = false;
	}
	//追尾中の速度計算
	if (workShot_.isTrack_ || worldTransform_.translation_.y >= collider_->GetRadius()) {
		velocity_ = diff.Normalize() * kSpeed;
	}

	worldTransform_.translation_ += velocity_;

}
