#include "Icicle.h"

#include "Easing.h"
#include "TextureManager.h"


void Icicle::Init(const std::shared_ptr<Model>& model) {

	obj_.reset(Object3d::Create(model));

	collider_.radius = 1.0f;

	particle_.reset(GPUParticle::Create(TextureManager::Load("circle.png"),5000));

	particle_->emitter_.direction = Vector3(0.0f, 0.0f, 1.0f);
	particle_->emitter_.angle = 360.0f;
	particle_->emitter_.color = Vector4(0.05f, 0.94f, 0.85f, 1.0f);
	particle_->emitter_.emitterType = 0;
	particle_->isLoop_ = true;

	hitEff_.reset(GPUParticle::Create(TextureManager::Load("circle.png"), 10000));

	hitEff_->isLoop_ = false;
	hitEff_->emitter_.count = 10000;
	hitEff_->emitter_.direction = Vector3(0.0f, 1.0f, 0.0f);
	hitEff_->emitter_.angle = 360.0f;
	hitEff_->emitter_.color = Vector4(0.05f, 0.94f, 0.85f, 1.0f);
	hitEff_->emitter_.lifeTime = 0.7f;
	hitEff_->emitter_.speed = 15.0f;
	hitEff_->emitter_.scale = 0.1f;
	hitEff_->emitter_.size = Vector3(0.0f, 0.0f, 0.0f);
	hitEff_->emitter_.emit = 0;
	hitEff_->emitter_.emitterType = 0;

}

void Icicle::Update() {

	if (phaseRequest_) {

		phase_ = phaseRequest_.value();

		phaseInitTable_[phase_]();

		phaseRequest_ = std::nullopt;

	}

	phaseUpdateTable_[phase_]();

	particle_->Update();
	hitEff_->Update();

	rotateMat_ = DirectionToDirection({ 0.0f,0.0f,1.0f }, direction_);
	
	obj_->worldTransform_.UpdateMatrixRotate(rotateMat_);
	particle_->emitter_.translate = obj_->GetWorldPos();
	ColliderUpdate();
}

void Icicle::Draw(const Camera& camera) {
	obj_->Draw(camera);
}

void Icicle::DrawParticle(const Camera& camera) {
	particle_->Draw(camera);
	hitEff_->Draw(camera);
}

void Icicle::OnCollision() {
	phaseRequest_ = Phase::kRoot;
	isLife_ = false;

	hitEff_->emitter_.emit = 1;
	hitEff_->emitter_.translate = obj_->GetWorldPos();

}

void Icicle::AttackStart() {
	phaseRequest_ = Phase::kCreate;
	isLife_ = true;
}

void Icicle::SetAttackData(const Vector3& pos, const Vector3& direction) {

	obj_->worldTransform_.translation_ = pos;
	direction_ = direction;
	rotateMat_ = DirectionToDirection({ 0.0f,0.0f,1.0f }, direction_);

}

void Icicle::RootInit() {

	obj_->worldTransform_.scale_ = {};
	particle_->emitter_.count = 0;

}

void Icicle::RootUpdate() {

	particle_->emitter_.frequencyTime = 0.0f;

}

void Icicle::CreateInit() {

	createData_.param_ = 0.0f;

	particle_->emitter_.count = 500;
	particle_->emitter_.frequency = 0.3f;
	particle_->emitter_.lifeTime = 2.0f;
	particle_->emitter_.scale = 1.0f;
	particle_->emitter_.size = Vector3(1.0f, 1.0f, 1.0f);
	particle_->emitter_.speed = -1.0f;
	

}

void Icicle::CreateUpdate() {

	createData_.param_ += 0.01f;
	createData_.param_ = std::clamp(createData_.param_, 0.0f, 1.0f);

	float T = Easing::easeInSine(createData_.param_);

	obj_->worldTransform_.scale_ = Lerp(T, Vector3(), Vector3(0.5f, 0.5f, 0.5f));

	if (createData_.param_ >= 1.0f) {
		phaseRequest_ = Phase::kWait;
	}

}

void Icicle::WaitInit() {

	waitData_.count_ = 0;

	particle_->emitter_.count = 0;

}

void Icicle::WaitUpdate() {

	particle_->emitter_.frequencyTime = 0.0f;

	waitData_.count_++;
	waitData_.count_ = std::clamp(waitData_.count_, 0, waitData_.waitTime_);

	if (waitData_.count_ >= waitData_.waitTime_) {
		phaseRequest_ = Phase::kShot;
	}

}

void Icicle::ShotInit() {

	shotData_.param_ = 0.0f;
	//targetDict_ = (target_->translation_ - obj_->worldTransform_.translation_);

	particle_->emitter_.count = 100;
	particle_->emitter_.angle = 30.0f;
	particle_->emitter_.frequency = 1.0f/60.0f;
	particle_->emitter_.lifeTime = 1.0f;
	particle_->emitter_.scale = 0.3f;
	particle_->emitter_.size = Vector3(0.5f, 0.5f, 0.5f);
	particle_->emitter_.speed = 0.0f;

}

void Icicle::ShotUpdate() {

	Vector3 diff = target_->translation_ - obj_->worldTransform_.translation_;
	float distance = diff.Length();

	if (distance >= shotData_.trackingDist_) {
		targetDict_ = (target_->translation_ - obj_->worldTransform_.translation_);
	}
	
	shotData_.param_ += 0.005f;

	direction_ = Lerp(shotData_.param_, direction_.Normalize(), targetDict_.Normalize());

	particle_->emitter_.direction = -direction_.Normalize();

	velocity_ = direction_ * speed_;

	obj_->worldTransform_.translation_ += velocity_;

	if (obj_->worldTransform_.translation_.y <= 0.0f) {
		OnCollision();
	}

}

