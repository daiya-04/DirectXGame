#include "MagicCannon.h"

#include "ColliderManager.h"
#include "ParticleManager.h"


MagicCannon::~MagicCannon() {
	DaiEngine::ColliderManager::GetInstance()->RemoveCollider(collider_.get());
}

void MagicCannon::Init() {

	worldTransform_.Init();


	collider_ = std::make_unique<DaiEngine::SphereCollider>();
	collider_->Init("PlayerAttack", worldTransform_, 0.5f);
	collider_->SetStayCallback([this](DaiEngine::Collider* other) {this->OnCollision(other); });
	collider_->SetValue("Damage", damage_);
	collider_->ColliderOff();
	DaiEngine::ColliderManager::GetInstance()->AddCollider(collider_.get());


	///エフェクト設定
	effect_ = ParticleManager::Load("MagicCannonEff");
	for (auto& [group, particle] : effect_) {
		particle->particleData_.isLoop_ = false;
	}

	endEff_ = ParticleManager::Load("MagicCannonEndEff");

}

void MagicCannon::Update() {

	//フェーズ切り替えの初期化
	if (phaseRequest_) {
		phase_ = phaseRequest_.value();

		phaseInitTable_[phase_]();

		phaseRequest_ = std::nullopt;
	}

	//フェーズ更新
	phaseUpdateTable_[phase_]();

	//行列更新
	worldTransform_.UpdateMatrix();

	collider_->Update();

	//エフェクト更新
	for (auto& [group, particle] : effect_) {
		particle->particleData_.emitter_.translate = worldTransform_.GetWorldPos();
		particle->Update();
	}
	for (auto& [group, particle] : endEff_) {
		particle->Update();
	}

}

void MagicCannon::DrawParticle(const DaiEngine::Camera& camera) {

	for (auto& [group, particle] : effect_) {
		particle->Draw(camera);
	}
	for (auto& [group, particle] : endEff_) {
		particle->Draw(camera);
	}

}

void MagicCannon::OnCollision(DaiEngine::Collider* other) {

	if (other->GetTag() == "Boss") {
		Dead();
	}

}

void MagicCannon::Dead() {

	collider_->ColliderOff();
	phaseRequest_ = Phase::kRoot;

	for (auto& [group, particle] : endEff_) {
		particle->Emit();
		particle->particleData_.emitter_.translate = worldTransform_.GetWorldPos();
	}

}

void MagicCannon::AttackStart(const Vector3& startPos, const Vector3& direction) {

	worldTransform_.translation_ = startPos;
	startPos_ = startPos;

	velocity_ = direction.Normalize() * speed_;

	phaseRequest_ = Phase::kShot;
	collider_->ColliderOn();

	worldTransform_.UpdateMatrix();
}

void MagicCannon::RootInit() {

	for (auto& [group, particle] : effect_) {
		particle->particleData_.isLoop_ = false;
	}

}

void MagicCannon::RootUpdate() {



}

void MagicCannon::ShotInit() {

	for (auto& [group, particle] : effect_) {
		particle->particleData_.isLoop_ = true;
	}

}

void MagicCannon::ShotUpdate() {
	//移動
	worldTransform_.translation_ += velocity_;

	//射程外で消える
	if ((worldTransform_.GetWorldPos() - startPos_).Length() >= firingRange_) {
		Dead();
	}

}
