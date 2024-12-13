#include "ElementBall.h"

#include "Easing.h"
#include "ShapesDraw.h"
#include "AnimationManager.h"
#include "TextureManager.h"
#include "ParticleManager.h"

void ElementBall::Init(std::shared_ptr<Model> model) {

	obj_.reset(Object3d::Create(model));
	animation_ = AnimationManager::Load(obj_->GetModel()->name_);

	collider_.radius = 1.3f;

	//エフェクト設定
	effect_ = ParticleManager::Load("FireBall");

	for (auto& [name, particle] : effect_) {
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
	obj_->worldTransform_.UpdateMatrix();

	if (animation_.IsPlaying()) {
		animation_.Play(obj_->GetModel()->rootNode_);
		obj_->worldTransform_.matWorld_ = animation_.GetLocalMatrix() * obj_->worldTransform_.matWorld_;
	}

	for (auto& [name, particle] : effect_) {
		particle->particleData_.emitter_.translate = GetWorldPos();
		particle->Update();
	}

	ColliderUpdate();
}

void ElementBall::Draw([[maybe_unused]]  const Camera& camera) {

#ifdef _DEBUG
	if (isLife_) {
		//ShapesDraw::DrawSphere(collider_, camera);
	}
#endif // _DEBUG

	//obj_->Draw(camera);

}

void ElementBall::DrawParticle(const Camera& camera) {
	for (auto& [name, particle] : effect_) {
		particle->Draw(camera);
	}
}

void ElementBall::OnCollision() {
	isLife_ = false;
	phaseRequest_ = Phase::kRoot;
}

void ElementBall::AttackStart() {

	phaseRequest_ = Phase::kSet;
	obj_->worldTransform_.scale_ = { 1.0f,1.0f,1.0f };
	isLife_ = true;

	for (auto& [name, particle] : effect_) {
		particle->particleData_.isLoop_ = true;
	}

	obj_->worldTransform_.UpdateMatrix();
}

void ElementBall::SetAttackData(const Vector3& startPos, uint32_t interval) {

	obj_->worldTransform_.translation_ = startPos;
	const uint32_t kFramePerSecond = 60;
	workCharge_.coolTime_ = kFramePerSecond * interval;
	for (auto& [name, particle] : effect_) {
		particle->particleData_.emitter_.translate = startPos;
	}

}

void ElementBall::RootInit() {

	obj_->worldTransform_.scale_ = {};
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
		obj_->worldTransform_.translation_.x = obj_->worldTransform_.matWorld_.m[3][0];
		obj_->worldTransform_.translation_.y = obj_->worldTransform_.matWorld_.m[3][1];
		obj_->worldTransform_.translation_.z = obj_->worldTransform_.matWorld_.m[3][2];
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

}

void ElementBall::ShotUpdate() {
	//進む方向を計算
	Vector3 diff = target_->translation_ - obj_->worldTransform_.translation_;
	float distance = diff.Length();
	const float kSpeed = 0.5f;
	//一定距離まで追尾
	if (distance < workShot_.trackingDist_) {
		workShot_.isTrack_ = false;
	}
	//追尾中の速度計算
	if (workShot_.isTrack_ || obj_->worldTransform_.translation_.y >= collider_.radius) {
		velocity_ = diff.Normalize() * kSpeed;
	}

	obj_->worldTransform_.translation_ += velocity_;

	if (obj_->worldTransform_.translation_.y < 0.0f) {
		isLife_ = false;
	}

}
