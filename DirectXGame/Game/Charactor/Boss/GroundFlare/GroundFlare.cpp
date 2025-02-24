#include "GroundFlare.h"

#include "Easing.h"
#include "TextureManager.h"
#include "ParticleManager.h"
#include "ColliderManager.h"

GroundFlare::~GroundFlare() {
	
}

void GroundFlare::Init(const std::shared_ptr<DaiEngine::Model>& model) {

	warningZone_.reset(DaiEngine::Object3d::Create(model));

	fire_ = ParticleManager::Load("GroundFire");
	for (auto& [group, particle] : fire_) {
		particle->particleData_.isLoop_ = false;
	}

	phase_ = Phase::kRoot;

	isAttack_ = false;
	isFire_ = false;

}

void GroundFlare::Update() {

	
	//フェーズ切り替えの初期化
	if (phaseRequest_) {

		phase_ = phaseRequest_.value();

		phaseInitTable_[phase_]();

		phaseRequest_ = std::nullopt;
	}
	//フェーズ更新
	phaseUpdateTable_[phase_]();

	for (auto& [group, particle] : fire_) {
		particle->Update();
	}

	warningZone_->worldTransform_.UpdateMatrix();

	
}

void GroundFlare::Draw(const DaiEngine::Camera& camera) {

	if (phase_==Phase::kWarning) {
		warningZone_->Draw(camera);
	}

}

void GroundFlare::DrawParticle(const DaiEngine::Camera& camera) {
	for (auto& [group, particle] : fire_) {
		particle->Draw(camera);
	}
}

void GroundFlare::AttackStart(const Vector3& firePos) {

	isAttack_ = true;
	phaseRequest_ = Phase::kWarning;

	warningZone_->worldTransform_.translation_ = firePos;
	for (auto& [group, particle] : fire_) {
		particle->particleData_.emitter_.translate = firePos;
	}

}

void GroundFlare::RootInit() {

}

void GroundFlare::RootUpdate() {

}

void GroundFlare::WarningInit() {

	workWarning_.param_ = 0.0f;

}

void GroundFlare::WarningUpdate() {

	workWarning_.param_ += workWarning_.speed_;
	float T = Easing::easeInSine(workWarning_.param_);
	//少しずつ大きくする
	warningZone_->worldTransform_.scale_ = Lerp(T, workWarning_.startScale_, workWarning_.endScale_);

	if (workWarning_.param_ >= 1.0f) {
		phaseRequest_ = Phase::kFire;
	}

}

void GroundFlare::FireInit() {

	isFire_ = true;
	workFire_.param_ = 0;

	for (auto& [group, particle] : fire_) {
		particle->particleData_.isLoop_ = true;
	}

}

void GroundFlare::FireUpdate() {

	workFire_.param_++;

	if (workFire_.param_ >= workFire_.fireCount_) {
		//通常状態に戻す
		phaseRequest_ = Phase::kRoot;
		//演出終了
		for (auto& [group, particle] : fire_) {
			particle->particleData_.isLoop_ = false;
		}
		isFire_ = false;
		isAttack_ = false;
	}

}
