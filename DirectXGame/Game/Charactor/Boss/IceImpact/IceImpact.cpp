#include "IceImpact.h"

#include "ColliderManager.h"
#include "ParticleManager.h"
#include "ModelManager.h"


IceImpact::~IceImpact() {
	DaiEngine::ColliderManager::GetInstance()->RemoveCollider(collider_.get());
}

void IceImpact::Init() {

	std::shared_ptr<DaiEngine::Model> model = DaiEngine::ModelManager::LoadOBJ("IceBlock");
	obj_.reset(DaiEngine::Object3d::Create(model));
	obj_->SetVisible(false);
	obj_->SetAplha(0.5f);
	obj_->SetSubsurfaceIntensity(0.4f);
	obj_->SetFresnelPower(4.0f);
	obj_->SetFresnelIntensity(1.0f);
	obj_->SetEnvReflectIntensity(0.05f);

	collider_ = std::make_unique<DaiEngine::SphereCollider>();
	collider_->Init("BossAttack", obj_->worldTransform_, 1.0f);
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
	obj_->worldTransform_.UpdateMatrix();

	collider_->Update();

	for (auto& [name, particle] : setEff_) {
		particle->Update();
	}
	for (auto& [name, particle] : impactEff_) {
		particle->Update();
	}
}

void IceImpact::Draw(const DaiEngine::Camera& camera) {

	obj_->Draw(camera, DaiEngine::BlendMode::kAlpha);

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

	float length = (target_->GetXZ() - obj_->GetWorldPos().GetXZ()).Length();

	if (length >= baseLength_) {
		phaseRequest_ = Phase::kShot;
	}
	else {
		phaseRequest_ = Phase::kCharge;
	}

	
	isAttack_ = true;

}

void IceImpact::SetAttackData(const Vector3& pos) {

	obj_->worldTransform_.translation_ = pos;

	for (auto& [name, particle] : setEff_) {
		particle->particleData_.emitter_.translate = pos;
	}
	for (auto& [name, particle] : impactEff_) {
		particle->particleData_.emitter_.translate = pos;
	}

	obj_->worldTransform_.UpdateMatrix();
}

void IceImpact::ObjRotation() {

	rotate_ += rotateSpeed_;
	rotate_ = std::fmodf(rotate_, 2.0f * std::numbers::pi_v<float>);

	obj_->worldTransform_.rotation_ = { rotate_,rotate_ ,rotate_ };

}

void IceImpact::IdleInit() {



}

void IceImpact::IdleUpdate() {



}

void IceImpact::ShotInit() {

	collider_->ColliderOn();
	collider_->SetRadius(shotRadius_);
	obj_->SetVisible(true);

	Vector3 sub = (target_->GetXZ() - obj_->GetWorldPos().GetXZ());
	float offset = 1.5f;
	workShot_.start = obj_->GetWorldPos() + (sub.Normalize() * offset);
	float halfLen = (sub.Length() / 2.0f);
	workShot_.end = obj_->GetWorldPos() + (sub.Normalize() * halfLen);
	workShot_.end.y = workShot_.impactPointHeight_;
	workShot_.param_ = 0.0f;

}

void IceImpact::ShotUpdate() {

	workShot_.param_ += workShot_.speed_;
	workShot_.param_ = std::clamp(workShot_.param_, 0.0f, 1.0f);

	obj_->worldTransform_.translation_ = Lerp(workShot_.param_, workShot_.start, workShot_.end);
	ObjRotation();

	if (workShot_.param_ >= 1.0f) {
		phaseRequest_ = Phase::kCharge;
		collider_->ColliderOff();
	}

}

void IceImpact::ChargeInit() {

	counter_ = 0;
	for (auto& [name, particle] : setEff_) {
		particle->particleData_.emitter_.translate = obj_->GetWorldPos();
		particle->particleData_.isLoop_ = true;
	}

}

void IceImpact::ChargeUpdate() {

	ObjRotation();

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

	ObjRotation();

	if (++counter_ >= waitTime_) {
		phaseRequest_ = Phase::kImpact;
	}

}

void IceImpact::ImpactInit() {

	counter_ = 0;
	collider_->ColliderOn();
	collider_->SetRadius(impactRadius_);
	for (auto& [name, particle] : impactEff_) {
		particle->particleData_.emitter_.translate = obj_->GetWorldPos();
		particle->Emit();
	}
	iceScar_->EffectStart(obj_->worldTransform_.GetWorldPos().GetXZ());
	iceScar_->HeightAdjustment(0.001f);

	obj_->SetVisible(false);

}

void IceImpact::ImpactUpdate() {

	if (++counter_ >= impactTime_) {
		phaseRequest_ = Phase::kIdle;
		collider_->ColliderOff();
		isAttack_ = false;
	}

}
