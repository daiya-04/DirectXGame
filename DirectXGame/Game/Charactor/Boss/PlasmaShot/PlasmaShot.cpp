#include "PlasmaShot.h"

#include "TextureManager.h"
#include "Easing.h"
#include "ParticleManager.h"


void PlasmaShot::Init(const std::shared_ptr<Model>& model) {

	obj_.reset(Object3d::Create(model));

	collider_.radius = 0.5f;

	///エフェクト設定

	createEff_ = ParticleManager::Load("PlasmaBallCreate");
	hitEff_ = ParticleManager::Load("PlasmaBallImpact");
	trailEff_ = ParticleManager::Load("PlasmaShotTrail");
	for (auto& [group, particle] : createEff_) {
		particle->particleData_.isLoop_ = false;
	}
	for (auto& [group, particle] : trailEff_) {
		particle->particleData_.isLoop_ = false;
	}

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

	//行列更新
	obj_->worldTransform_.UpdateMatrix();

	//エフェクト更新
	for (auto& [group, particle] : hitEff_) {
		particle->Update();
	}
	for (auto& [group, particle] : createEff_) {
		particle->Update();
	}
	for (auto& [group, particle] : trailEff_) {
		particle->particleData_.emitter_.translate = obj_->GetWorldPos();
		particle->Update();
	}
	
	ColliderUpdate();
}

void PlasmaShot::ColliderUpdate() {
	collider_.center = obj_->GetWorldPos();
}

void PlasmaShot::Draw(const Camera& camera) {
	obj_->Draw(camera);
}

void PlasmaShot::DrawParticle(const Camera& camera) {
	for (auto& [group, particle] : hitEff_) {
		particle->Draw(camera);
	}
	for (auto& [group, particle] : createEff_) {
		particle->Draw(camera);
	}
	for (auto& [group, particle] : trailEff_) {
		particle->Draw(camera);
	}
}


void PlasmaShot::OnCollision() {
	phaseRequest_ = Phase::kRoot;
	isLife_ = false;

	
	//ヒットエフェクト開始
	for (auto& [group, particle] : trailEff_) {
		particle->particleData_.isLoop_ = false;
	}
	for (auto& [group, particle] : hitEff_) {
		particle->Emit();
		particle->particleData_.emitter_.translate = obj_->GetWorldPos();
	}
}

void PlasmaShot::AttackStart() {
	phaseRequest_ = Phase::kCreate;
	isLife_ = true;
}

void PlasmaShot::SetAttackData(const Vector3& pos, float interval) {

	obj_->worldTransform_.translation_ = pos;
	const float kFramePerSecond = 60.0f;
	waitData_.waitTime_ = static_cast<int32_t>(kFramePerSecond * interval);

}

void PlasmaShot::RootInit() {

	obj_->worldTransform_.scale_ = {};

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
	for (auto& [group, particle] : createEff_) {
		particle->particleData_.isLoop_ = true;
		particle->particleData_.emitter_.translate = obj_->GetWorldPos();
	}

}

void PlasmaShot::WaitUpdate() {

	waitData_.count_++;
	waitData_.count_ = std::clamp(waitData_.count_, 0, waitData_.waitTime_);

	if (waitData_.count_ >= waitData_.waitTime_) {
		phaseRequest_ = Phase::kShot;
	}

}

void PlasmaShot::ShotInit() {
	const float objScale = 0.5f;
	obj_->worldTransform_.scale_ = { objScale,objScale,objScale };
	//ターゲットの方向
	targetDict_ = target_->translation_ - obj_->worldTransform_.translation_;

	for (auto& [group, particle] : createEff_) {
		particle->particleData_.isLoop_ = false;
	}
	for (auto& [group, particle] : trailEff_) {
		particle->particleData_.isLoop_ = true;
	}


}

void PlasmaShot::ShotUpdate() {
	//速度の計算
	velocity_ = targetDict_.Normalize() * speed_;

	obj_->worldTransform_.translation_ += velocity_;
	//地面より下にはいかない
	if (obj_->worldTransform_.translation_.y <= 0.0f) {
		OnCollision();
	}

}
