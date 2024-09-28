#include "PlasmaShot.h"

#include "TextureManager.h"
#include "Easing.h"



void PlasmaShot::Init(const std::shared_ptr<Model>& model) {

	obj_.reset(Object3d::Create(model));

	collider_.radius = 0.5f;

	particle_.reset(GPUParticle::Create(TextureManager::Load("PlasmaParticle.png"), 2000));
	particle_->emitter_.direction = Vector3(0.0f, 0.0f, 1.0f);
	particle_->emitter_.angle = 360.0f;
	particle_->emitter_.color = Vector4(0.43f, 0.2f, 0.67f, 1.0f);
	particle_->emitter_.emitterType = 0;
	particle_->isLoop_ = true;

	hitEff_.reset(GPUParticle::Create(TextureManager::Load("PlasmaParticle.png"), 10000));
	
	hitEff_->emitter_.direction = Vector3(0.0f, 1.0f, 0.0f);
	hitEff_->emitter_.angle = 360.0f;
	hitEff_->emitter_.color = Vector4(0.43f, 0.2f, 0.67f, 1.0f);
	hitEff_->emitter_.size = Vector3(0.0f, 0.0f, 0.0f);
	hitEff_->emitter_.lifeTime = 1.0f;
	hitEff_->emitter_.count = 10000;
	hitEff_->emitter_.scale = 0.05f;
	hitEff_->emitter_.emit = 0;
	hitEff_->emitter_.speed = 10.0f;
	hitEff_->emitter_.emitterType = 0;

	hitEff_->isLoop_ = false;

}

void PlasmaShot::Update() {

	if (phaseRequest_) {

		phase_ = phaseRequest_.value();

		phaseInitTable_[phase_]();

		phaseRequest_ = std::nullopt;

	}

	phaseUpdateTable_[phase_]();

	particle_->Update();
	hitEff_->Update();

	obj_->worldTransform_.UpdateMatrix();
	particle_->emitter_.translate = obj_->GetWorldPos();
	ColliderUpdate();
}

void PlasmaShot::ColliderUpdate() {
	collider_.center = obj_->GetWorldPos();
}

void PlasmaShot::Draw(const Camera& camera) {
	obj_->Draw(camera);
}

void PlasmaShot::DrawParticle(const Camera& camera) {
	particle_->Draw(camera);
	hitEff_->Draw(camera);
}


void PlasmaShot::OnCollision() {
	phaseRequest_ = Phase::kRoot;
	isLife_ = false;
	
	hitEff_->emitter_.emit = 1;
	hitEff_->emitter_.translate = obj_->GetWorldPos();
}

void PlasmaShot::AttackStart() {
	phaseRequest_ = Phase::kCreate;
	isLife_ = true;
}

void PlasmaShot::SetAttackData(const Vector3& pos, float interval) {

	obj_->worldTransform_.translation_ = pos;
	particle_->emitter_.translate = pos;
	waitData_.waitTime_ = int32_t(60.0f * interval);

}

void PlasmaShot::RootInit() {

	obj_->worldTransform_.scale_ = {};
	particle_->emitter_.count = 0;

}

void PlasmaShot::RootUpdate() {

	particle_->emitter_.frequencyTime = 0.0f;

}

void PlasmaShot::CreateInit() {

	particle_->emitter_.count = 50;
	particle_->emitter_.frequency = 2.0f / 60.0f;
	particle_->emitter_.lifeTime = 1.0f;
	particle_->emitter_.scale = 0.5f;
	particle_->emitter_.size = Vector3(1.0f, 1.0f, 1.0f);
	particle_->emitter_.speed = 1.0f;

}

void PlasmaShot::CreateUpdate() {

	phaseRequest_ = Phase::kWait;

}

void PlasmaShot::WaitInit() {

	waitData_.count_ = 0;

}

void PlasmaShot::WaitUpdate() {

	waitData_.count_++;
	waitData_.count_ = std::clamp(waitData_.count_, 0, waitData_.waitTime_);

	if (waitData_.count_ >= waitData_.waitTime_) {
		phaseRequest_ = Phase::kShot;
	}

}

void PlasmaShot::ShotInit() {

	obj_->worldTransform_.scale_ = { 0.5f,0.5f,0.5f };

	targetDict_ = target_->translation_ - obj_->worldTransform_.translation_;

	particle_->emitter_.count = 50;
	particle_->emitter_.frequency = 1.0f / 60.0f;
	particle_->emitter_.lifeTime = 1.0f;
	particle_->emitter_.scale = 0.3f;
	particle_->emitter_.size = Vector3(0.3f, 0.3f, 0.3f);
	particle_->emitter_.speed = 0.0f;
	particle_->emitter_.direction = -targetDict_.Normalize();

}

void PlasmaShot::ShotUpdate() {

	velocity_ = targetDict_.Normalize() * speed_;

	obj_->worldTransform_.translation_ += velocity_;

	if (obj_->worldTransform_.translation_.y <= 0.0f) {
		OnCollision();
	}

}
