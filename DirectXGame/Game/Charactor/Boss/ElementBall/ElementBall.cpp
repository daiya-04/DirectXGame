#include "ElementBall.h"

#include "Easing.h"
#include "ShapesDraw.h"
#include "AnimationManager.h"
#include "TextureManager.h"
#include "ParticleManager.h"

void ElementBall::Init(std::shared_ptr<Model> model) {

	obj_.reset(Object3d::Create(model));
	animation_ = AnimationManager::Load(obj_->GetModel()->name_);

	core_.reset(GPUParticle::Create(TextureManager::Load("circle.png"), 20000));
	core_->SetParticleData(ParticleManager::Load("FireBallCore"));
	core_->particleData_.isLoop_ = false;

	smoke_.reset(GPUParticle::Create(TextureManager::Load("circle.png"), 1024));
	smoke_->SetParticleData(ParticleManager::Load("FireBallSmoke"));
	smoke_->particleData_.isLoop_ = false;
	

	isLife_ = false;
	preIsLife_ = false;

}

void ElementBall::Update() {

	preIsLife_ = isLife_;

	if (phaseRequest_) {

		phase_ = phaseRequest_.value();

		phaseInitTable_[phase_]();

		phaseRequest_ = std::nullopt;
	}

	phaseUpdateTable_[phase_]();

	//行列更新
	obj_->worldTransform_.UpdateMatrix();

	if (animation_.IsPlaying()) {
		animation_.Play(obj_->GetModel()->rootNode_);
		obj_->worldTransform_.matWorld_ = animation_.GetLocalMatrix() * obj_->worldTransform_.matWorld_;
	}

	core_->particleData_.emitter_.translate = GetWorldPos();
	smoke_->particleData_.emitter_.translate = GetWorldPos();
	core_->Update();
	smoke_->Update();

	ColliderUpdate();
}

void ElementBall::Draw(const Camera& camera) {

#ifdef _DEBUG
	if (isLife_) {
		//ShapesDraw::DrawSphere(collider_, camera);
	}
#endif // _DEBUG

	//obj_->Draw(camera);

}

void ElementBall::DrawParticle(const Camera& camera) {
	core_->Draw(camera);
	smoke_->Draw(camera);
}

void ElementBall::OnCollision() {
	isLife_ = false;
	phaseRequest_ = Phase::kRoot;
}

void ElementBall::AttackStart() {

	phaseRequest_ = Phase::kSet;
	obj_->worldTransform_.translation_ = workSet_.start;
	obj_->worldTransform_.scale_ = { 1.0f,1.0f,1.0f };
	isLife_ = true;

	core_->particleData_.isLoop_ = true;
	smoke_->particleData_.isLoop_ = true;

	obj_->worldTransform_.UpdateMatrix();
}

void ElementBall::SetAttackData(const Vector3& startPos, uint32_t interval) {

	workSet_.start = startPos;
	workCharge_.coolTime = 60 * interval;
	core_->particleData_.emitter_.translate = startPos;
	smoke_->particleData_.emitter_.translate = startPos;

}

void ElementBall::RootInit() {

	obj_->worldTransform_.scale_ = {};
	core_->particleData_.isLoop_ = false;
	smoke_->particleData_.isLoop_ = false;
	animation_.TimeReset();

}

void ElementBall::RootUpdate() {



}

void ElementBall::SetInit() {

	
	animation_.Start(false);

}

void ElementBall::SetUpdate() {

	if (!animation_.IsPlaying()) {
		phaseRequest_ = Phase::kCharge;
		animation_.End();
		obj_->worldTransform_.translation_.x = obj_->worldTransform_.matWorld_.m[3][0];
		obj_->worldTransform_.translation_.y = obj_->worldTransform_.matWorld_.m[3][1];
		obj_->worldTransform_.translation_.z = obj_->worldTransform_.matWorld_.m[3][2];
	}

}

void ElementBall::ChargeInit() {

	workCharge_.param = 0;

}

void ElementBall::ChargeUpdate() {

	if (++workCharge_.param >= workCharge_.coolTime) {
		phaseRequest_ = Phase::kShot;
	}

}

void ElementBall::ShotInit() {

	workShot_.move = {};
	workShot_.isTrack = true;

}

void ElementBall::ShotUpdate() {

	Vector3 diff = target_->translation_ - obj_->worldTransform_.translation_;
	float distance = diff.Length();
	const float kSpeed = 0.5f;

	if (distance < workShot_.trackingDist) {
		workShot_.isTrack = false;
	}

	if (workShot_.isTrack || obj_->worldTransform_.translation_.y >= 2.0f) {
		workShot_.move = diff.Normalize() * kSpeed;
	}

	obj_->worldTransform_.translation_ += workShot_.move;

	if (obj_->worldTransform_.translation_.y < 0.0f) {
		isLife_ = false;
	}

}
