#include "IceImpact.h"

#include "ColliderManager.h"
#include "ParticleManager.h"


IceImpact::~IceImpact() {
	DaiEngine::ColliderManager::GetInstance()->RemoveCollider(collider_.get());
}

void IceImpact::Init() {

	worldTrabsform_.Init();

	collider_ = std::make_unique<DaiEngine::SphereCollider>();
	collider_->Init("BossAttack", worldTrabsform_, 15.0f);
	collider_->SetStayCallback([this](DaiEngine::Collider* other) {this->OnCollision(other); });
	DaiEngine::ColliderManager::GetInstance()->AddCollider(collider_.get());

	setEff_ = ParticleManager::Load("IceImpactSet");
	impactEff_ = ParticleManager::Load("IceImpact");

	for (auto& [name, particle] : setEff_) {
		particle->particleData_.isLoop_ = false;
	}

	isAttack_ = false;

}

void IceImpact::Update() {

	//フェーズの切り替えと初期化
	if (phaseRequest_) {
		phase_ = phaseRequest_.value();

		phaseInitTable_[phase_]();

		phaseRequest_ = std::nullopt;
	}
	//フェーズ更新
	phaseUpdateTable_[phase_]();

	//行列更新
	worldTrabsform_.UpdateMatrix();

	collider_->Update();

	for (auto& [name, particle] : setEff_) {
		particle->Update();
	}
	for (auto& [name, particle] : impactEff_) {
		particle->Update();
	}
}

void IceImpact::DrawParticle(const DaiEngine::Camera& camera) {
	for (auto& [name, particle] : setEff_) {
		particle->Draw(camera);
	}
	for (auto& [name, particle] : impactEff_) {
		particle->Draw(camera);
	}
}

void IceImpact::OnCollision(DaiEngine::Collider* other) {

	if (other->GetTag() == "Player") {
		collider_->ColliderOff();
	}

}

void IceImpact::AttackStart() {

	phaseRequest_ = Phase::kCharge;
	isAttack_ = true;

}

void IceImpact::SetAttackData(const Vector3& pos) {

	worldTrabsform_.translation_ = pos;

	for (auto& [name, particle] : setEff_) {
		particle->particleData_.emitter_.translate = pos;
	}
	for (auto& [name, particle] : impactEff_) {
		particle->particleData_.emitter_.translate = pos;
	}

}

void IceImpact::IdleInit() {



}

void IceImpact::IdleUpdate() {



}

void IceImpact::ChargeInit() {

	counter_ = 0;
	for (auto& [name, particle] : setEff_) {
		particle->particleData_.isLoop_ = true;
	}

}

void IceImpact::ChargeUpdate() {

	if (++counter_ >= chargeTime_) {
		phaseRequest_ = Phase::kWait;
		for (auto& [name, particle] : setEff_) {
			particle->particleData_.isLoop_ = false;
		}
	}

}

void IceImpact::WaitInit() {

	counter_ = 0;

}

void IceImpact::WaitUpdate() {

	if (++counter_ >= waitTime_) {
		phaseRequest_ = Phase::kImpact;
	}

}

void IceImpact::ImpactInit() {

	counter_ = 0;
	collider_->ColliderOn();
	for (auto& [name, particle] : impactEff_) {
		particle->Emit();
	}
	iceScar_->EffectStart(worldTrabsform_.GetWorldPos().GetXZ());
	iceScar_->HeightAdjustment(0.001f);

}

void IceImpact::ImpactUpdate() {

	if (++counter_ >= impactTime_) {
		phaseRequest_ = Phase::kIdle;
		collider_->ColliderOff();
		isAttack_ = false;
	}

}
