#include "GroundBurst.h"

#include "ColliderManager.h"
#include "ParticleManager.h"


GroundBurst::~GroundBurst() {
	DaiEngine::ColliderManager::GetInstance()->RemoveCollider(collider_.get());
}

void GroundBurst::Init() {

	worldTransform_.Init();

	///コライダー初期化
	collider_ = std::make_unique<DaiEngine::OBBCollider>();
	collider_->Init("PlayerAttack", worldTransform_, Vector3(1.0f, 2.0f, 1.0f));
	collider_->SetPosition(Vector3(0.0f, collider_->GetSize().y, 0.0f));
	collider_->SetCallbackFunc([this](DaiEngine::Collider* other) { OnCollision(other); });
	DaiEngine::ColliderManager::GetInstance()->AddCollider(collider_.get());
	///

	///エフェクト設定

	effect_ = ParticleManager::Load("GroundBurst");

	///

	phaseRequest_ = Phase::kIdle;

	isAttack_ = false;

}

void GroundBurst::Update() {

	//フェーズの切り替えと初期化
	if (phaseRequest_) {
		phase_ = phaseRequest_.value();

		phaseInitTable_[phase_]();

		phaseRequest_ = std::nullopt;
	}

	//フェーズ更新
	phaseUpdateTable_[phase_]();

	//行列更新
	worldTransform_.UpdateMatrixRotate(rotateMat_);
	collider_->Update(rotateMat_);

	//エフェクト更新
	for (auto& [group, particle] : effect_) {
		particle->Update();
	}

}

void GroundBurst::DrawParticle(const DaiEngine:: Camera& camera) {
	for (auto& [group, particle] : effect_) {
		particle->Draw(camera);
	}
}

void GroundBurst::OnCollision([[maybe_unused]] DaiEngine::Collider* other) {

	if (other->GetTag() == "Boss") {
		Dead();
	}

}

void GroundBurst::Dead() {

	isAttack_ = false;
	phaseRequest_ = Phase::kIdle;

	collider_->ColliderOff();

}

void GroundBurst::AttackStart(const Vector3& attackPos) {

	phaseRequest_ = Phase::kAttack;
	collider_->ColliderOn();

	worldTransform_.translation_ = attackPos;
	worldTransform_.translation_.y = 0.01f;

	worldTransform_.UpdateMatrixRotate(rotateMat_);
}

void GroundBurst::IdleInit() {



}

void GroundBurst::IdleUpdate() {



}

void GroundBurst::AttackInit() {

	for (auto& [group, particle] : effect_) {
		particle->Emit();
		particle->particleData_.emitter_.translate = worldTransform_.GetWorldPos();
	}

}

void GroundBurst::AttackUpdate() {

	if (++count_ >= attackTime_) {
		Dead();
	}

}
