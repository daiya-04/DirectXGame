#include "GroundFlare.h"

#include "Easing.h"
#include "TextureManager.h"


GroundFlare* GroundFlare::GetInstance() {
	static GroundFlare instance;

	return &instance;
}

void GroundFlare::Init(std::shared_ptr<Model> model) {

	for (auto& warningZone : warningZones_) {
		warningZone.reset(Object3d::Create(model));
	}

	for (auto& particle : particles_) {
		particle.reset(GPUParticle::Create(TextureManager::Load("FireParticle.png"), 5000));
		particle->emitter_.direction = Vector3(0.0f, 1.0f, 0.0f).Normalize();
		particle->emitter_.angle = 15.0f;
	}


	colliderHSize_ = {2.0f,3.0f,2.0f};

	offset_[0] = {}; //中心
	offset_[1] = { -1.5f,0.0f,1.5f }; //左上
	offset_[2] = { 1.5f,0.0f,1.5f }; //右上
	offset_[3] = { -1.5f,0.0f,-1.5f }; //左下
	offset_[4] = { 1.5f,0.0f,-1.5f }; //右下

}

void GroundFlare::Update() {

	preIsAttack_ = isAttack_;
	preIsHit_ = isHit_;

	if (phaseRequest_) {

		phase_ = phaseRequest_.value();

		switch (phase_) {
		case Phase::kRoot:
			RootInit();
			break;
		case Phase::kWarning:
			WarningInit();
			break;
		case Phase::kFire:
			FireInit();
			break;
		}

		phaseRequest_ = std::nullopt;
	}

	switch (phase_) {
	case Phase::kRoot:
		RootUpdate();
		break;
	case Phase::kWarning:
		WarningUpdate();
		break;
	case Phase::kFire:
		FireUpdate();
		break;
	}

	for (auto& particle : particles_) {
		particle->Update();
	}

	for (auto& warningZone : warningZones_) {
		warningZone->worldTransform_.UpdateMatrix();
	}

	ColliderUpdate();
}

void GroundFlare::ColliderUpdate() {

	collider_.max = colliderCenter_ + colliderHSize_;
	collider_.min = colliderCenter_ - colliderHSize_;

}

void GroundFlare::Draw(const Camera& camera) {

	if (phase_==Phase::kWarning) {
		for (auto& warningZone : warningZones_) {
			warningZone->Draw(camera);
		}
	}

}

void GroundFlare::DrawParticle(const Camera& camera) {

	for (auto& particle : particles_) {
		particle->Draw(camera);
	}

}

void GroundFlare::OnCollision() {
	isHit_ = false;
}

void GroundFlare::AttackStart() {

	phaseRequest_ = Phase::kWarning;

}

void GroundFlare::RootInit() {

	for (auto& particle : particles_) {
		particle->emitter_.count = 0;
	}

	isHit_ = false;
	isAttack_ = false;

}

void GroundFlare::RootUpdate() {

	for (auto& particle : particles_) {
		particle->emitter_.frequencyTime = 0.0f;
	}

}

void GroundFlare::WarningInit() {

	centerPos_ = target_->translation_;
	centerPos_.y = 0.01f;
	colliderCenter_ = centerPos_ + Vector3(0.0f, colliderHSize_.y, 0.0f);

	workWarning_.param_ = 0.0f;

	
	for (size_t index = 0; index < 5; index++) {
		particles_[index]->emitter_.translate = centerPos_ + offset_[index];
		warningZones_[index]->worldTransform_.translation_ = centerPos_ + offset_[index];
	}

	isAttack_ = true;

}

void GroundFlare::WarningUpdate() {

	workWarning_.param_ += 0.01f;
	float T = Easing::easeInSine(workWarning_.param_);

	for (auto& warningZone : warningZones_) {
		warningZone->worldTransform_.scale_ = Lerp(T, Vector3(0.0f, 1.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f));
	}

	if (workWarning_.param_ >= 1.0f) {
		phaseRequest_ = Phase::kFire;
	}

}

void GroundFlare::FireInit() {

	isHit_ = true;
	workFire_.param_ = 0;

	for (auto& particle : particles_) {
		particle->emitter_.scale = 0.7f;
		particle->emitter_.size = 0.7f;
		particle->emitter_.frequency = 0.1f;
		particle->emitter_.count = 50;
	}

}

void GroundFlare::FireUpdate() {

	workFire_.param_++;

	if (workFire_.param_ >= workFire_.shotCount_) {
		for (auto& particle : particles_) {
			particle->emitter_.frequencyTime = 0.0f;
		}
	}

	if (workFire_.param_ >= workFire_.fireCount_) {
		phaseRequest_ = Phase::kRoot;

	}

}
