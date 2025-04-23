#include "PlasmaShot.h"

#include "TextureManager.h"
#include "Easing.h"
#include "ParticleManager.h"
#include "ColliderManager.h"
#include "ModelManager.h"

PlasmaShot::~PlasmaShot() {
	DaiEngine::ColliderManager::GetInstance()->RemoveCollider(collider_.get());
}

void PlasmaShot::Init() {

	std::shared_ptr<DaiEngine::Model> model = DaiEngine::ModelManager::LoadOBJ("PlasmaBall");
	obj_.reset(DaiEngine::Object3d::Create(model));
	obj_->SetAplha(0.6f);

	collider_ = std::make_unique<DaiEngine::SphereCollider>();
	collider_->Init("BossAttack", obj_->worldTransform_, 0.5f);
	collider_->SetStayCallback([this](DaiEngine::Collider* other) {this->OnCollision(other); });
	DaiEngine::ColliderManager::GetInstance()->AddCollider(collider_.get());

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
	
	collider_->Update();
}

void PlasmaShot::Draw(const DaiEngine::Camera& camera) {
	obj_->Draw(camera);
}

void PlasmaShot::DrawParticle(const DaiEngine::Camera& camera) {
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


void PlasmaShot::OnCollision(DaiEngine::Collider* other) {
	if (other->GetTag() == "Player" || other->GetTag() == "Ground") {
		Dead();
	}
}

void PlasmaShot::Dead() {
	phaseRequest_ = Phase::kRoot;
	isLife_ = false;

	collider_->ColliderOff();

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
	targetDict_ = *target_ - obj_->worldTransform_.translation_;

	for (auto& [group, particle] : createEff_) {
		particle->particleData_.isLoop_ = false;
	}
	for (auto& [group, particle] : trailEff_) {
		particle->particleData_.isLoop_ = true;
	}

	collider_->ColliderOn();

}

void PlasmaShot::ShotUpdate() {
	//速度の計算
	velocity_ = targetDict_.Normalize() * speed_;

	obj_->worldTransform_.translation_ += velocity_;

}
