#include "ElementBall.h"

#include "Easing.h"
#include "ShapesDraw.h"
#include "AnimationManager.h"
#include "TextureManager.h"

void ElementBall::Init(std::shared_ptr<Model> model) {

	obj_.reset(Object3d::Create(model));
	animation_ = AnimationManager::Load(obj_->GetModel()->name_);

	particle_.reset(GPUParticle::Create(TextureManager::Load("Steam.png"), 50000));
	
	particle_->emitter_.color = Vector4(0.89f, 0.27f, 0.03f, 1.0f);
	particle_->emitter_.emitterType = GPUParticle::EmitShape::Sphere;
	particle_->emitter_.frequency = 1.0f / 60.0f;
	particle_->emitter_.radius = 0.3f;

	particle_->overLifeTime_.isAlpha = 1;
	particle_->overLifeTime_.midAlpha = 1.0f;

	particle_->overLifeTime_.isScale = 1;
	particle_->overLifeTime_.startScale = 0.3f;

	particle_->overLifeTime_.isColor = 1;
	particle_->overLifeTime_.startColor = Vector3(0.89f, 0.27f, 0.03f);
	particle_->overLifeTime_.endColor = Vector3(1.0f, 0.0f, 0.0f);

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
		animation_.Play(obj_->GetModel()->rootNode_, false);
		obj_->worldTransform_.matWorld_ = animation_.GetLocalMatrix() * obj_->worldTransform_.matWorld_;
	}

	particle_->emitter_.translate = GetWorldPos();
	particle_->Update();

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
	particle_->Draw(camera);
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

	particle_->isLoop_ = true;

	obj_->worldTransform_.UpdateMatrix();
}

void ElementBall::SetAttackData(const Vector3& startPos, uint32_t interval) {

	workSet_.start = startPos;
	workCharge_.coolTime = 60 * interval;
	particle_->emitter_.translate = startPos;

}

void ElementBall::RootInit() {

	obj_->worldTransform_.scale_ = {};
	particle_->emitter_.count = 0;
	particle_->isLoop_ = false;
	animation_.TimeReset();

}

void ElementBall::RootUpdate() {

	particle_->emitter_.frequencyTime = 0.0f;

}

void ElementBall::SetInit() {

	
	animation_.Start();

	particle_->emitter_.count = 150;
	particle_->emitter_.lifeTime = 1.0f;
	particle_->emitter_.speed = 0.0f;
	particle_->emitter_.radius = 0.5f;

	particle_->overLifeTime_.startAlpha = 1.0f;

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

	particle_->emitter_.count = 150;
	particle_->emitter_.speed = 1.0f;
	particle_->emitter_.scale = 0.3f;
	particle_->emitter_.radius = 0.7f;
	
	particle_->overLifeTime_.isTransSpeed = 1;
	particle_->overLifeTime_.startSpeed = 3.0f;
	particle_->overLifeTime_.startAlpha = 0.0f;

}

void ElementBall::ChargeUpdate() {

	if (++workCharge_.param >= workCharge_.coolTime) {
		phaseRequest_ = Phase::kShot;
	}

}

void ElementBall::ShotInit() {

	workShot_.move = {};
	workShot_.isTrack = true;

	particle_->emitter_.radius = 0.5f;
	particle_->emitter_.speed = 0.0f;
	particle_->emitter_.scale = 0.3f;
	particle_->emitter_.lifeTime = 0.5f;
	particle_->emitter_.count = 150;
	

	particle_->overLifeTime_.isTransSpeed = 0;

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
