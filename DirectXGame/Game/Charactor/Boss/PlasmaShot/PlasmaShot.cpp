#include "PlasmaShot.h"

#include "TextureManager.h"
#include "Easing.h"
#include "ParticleManager.h"


void PlasmaShot::Init(const std::shared_ptr<Model>& model) {

	obj_.reset(Object3d::Create(model));

	collider_.radius = 0.5f;

	///エフェクト設定
	particle_.reset(GPUParticle::Create(TextureManager::Load("PlasmaParticle.png"), 2000));
	particle_->SetParticleData(ParticleManager::Load("PlasmaShotTrail"));
	particle_->particleData_.isLoop_ = false;

	hitEff_.reset(GPUParticle::Create(TextureManager::Load("PlasmaParticle.png"), 1024));
	hitEff_->SetParticleData(ParticleManager::Load("PlasmaShotImpact"));

	hitSpark_.reset(GPUParticle::Create(TextureManager::Load("circle.png"), 1024));
	hitSpark_->SetParticleData(ParticleManager::Load("PlasmaShotImpactSpark"));

	createEff_.reset(GPUParticle::Create(TextureManager::Load("circle.png"), 1024));
	createEff_->SetParticleData(ParticleManager::Load("PlasmaShotCreate"));
	///

}

void PlasmaShot::Update() {
	//フェーズ切り替えの初期化
	if (phaseRequest_) {
		phase_ = phaseRequest_.value();

		phaseInitTable_[phase_]();

		phaseRequest_ = std::nullopt;
	}
	//フェーズ更新
	phaseUpdateTable_[phase_]();


	obj_->worldTransform_.UpdateMatrix();
	particle_->particleData_.emitter_.translate = obj_->GetWorldPos();

	particle_->Update();
	hitEff_->Update();
	hitSpark_->Update();
	createEff_->Update();
	
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
	hitSpark_->Draw(camera);
	createEff_->Draw(camera);
}


void PlasmaShot::OnCollision() {
	phaseRequest_ = Phase::kRoot;
	isLife_ = false;

	particle_->particleData_.isLoop_ = false;
	//ヒットエフェクト開始
	hitEff_->particleData_.emitter_.emit = 1;
	hitSpark_->particleData_.emitter_.emit = 1;
	hitEff_->particleData_.emitter_.translate = obj_->GetWorldPos();
	hitSpark_->particleData_.emitter_.translate = obj_->GetWorldPos();
}

void PlasmaShot::AttackStart() {
	phaseRequest_ = Phase::kCreate;
	isLife_ = true;
}

void PlasmaShot::SetAttackData(const Vector3& pos, float interval) {

	obj_->worldTransform_.translation_ = pos;
	waitData_.waitTime_ = int32_t(60.0f * interval);

}

void PlasmaShot::RootInit() {

	obj_->worldTransform_.scale_ = {};
	particle_->particleData_.isLoop_ = false;
	createEff_->particleData_.isLoop_ = false;

}

void PlasmaShot::RootUpdate() {



}

void PlasmaShot::CreateInit() {

}

void PlasmaShot::CreateUpdate() {

	phaseRequest_ = Phase::kWait;

}

void PlasmaShot::WaitInit() {

	waitData_.count_ = 0;
	createEff_->particleData_.isLoop_ = true;
	createEff_->particleData_.emitter_.translate = obj_->GetWorldPos();

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
	//ターゲットの方向
	targetDict_ = target_->translation_ - obj_->worldTransform_.translation_;

	particle_->particleData_.isLoop_ = true;

	createEff_->particleData_.isLoop_ = false;

}

void PlasmaShot::ShotUpdate() {

	velocity_ = targetDict_.Normalize() * speed_;

	obj_->worldTransform_.translation_ += velocity_;
	//地面より下にはいかない
	if (obj_->worldTransform_.translation_.y <= 0.0f) {
		OnCollision();
	}

}
