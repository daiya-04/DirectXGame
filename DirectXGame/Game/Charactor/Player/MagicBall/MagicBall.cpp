#include "MagicBall.h"

#include "TextureManager.h"
#include "ColliderManager.h"
#include "ParticleManager.h"
#include "ModelManager.h"

MagicBall::~MagicBall() {
	DaiEngine::ColliderManager::GetInstance()->RemoveCollider(collider_.get());
}

void MagicBall::Init() {

	worldTransform_.Init();

	collider_ = std::make_unique<DaiEngine::SphereCollider>();
	collider_->Init("PlayerAttack", worldTransform_, 0.3f);
	collider_->SetStayCallback([this](DaiEngine::Collider* other) {this->OnCollision(other); });
	collider_->SetValue("Damage", damage_);
	collider_->ColliderOff();
	DaiEngine::ColliderManager::GetInstance()->AddCollider(collider_.get());

	///エフェクト設定

	trailEff_ = ParticleManager::Load("playerAttackTrail");
	for (auto& [group, particle] : trailEff_) {
		particle->particleData_.isLoop_ = false;
	}

	endEff_ = ParticleManager::Load("PlayerAttackEnd");

	///

}

void MagicBall::Update() {

	//フェーズ切り替えの初期化
	if (phaseRequest_) {
		phase_ = phaseRequest_.value();

		phaseInitTable_[phase_]();

		phaseRequest_ = std::nullopt;
	}
	//フェーズ更新
	phaseUpdateTable_[phase_]();


	//行列計算
	worldTransform_.UpdateMatrix();
	collider_->Update();

	//エフェクト更新
	for (auto& [group, particle] : trailEff_) {
		particle->particleData_.emitter_.translate = worldTransform_.GetWorldPos();
		particle->Update();
	}
	for (auto& [group, particle] : endEff_) {
		particle->Update();
	}

}

void MagicBall::DrawParticle(const DaiEngine::Camera& camera) {
	for (auto& [group, particle] : trailEff_) {
		particle->Draw(camera);
	}
	for (auto& [group, particle] : endEff_) {
		particle->Draw(camera);
	}
}

void MagicBall::OnCollision([[maybe_unused]] DaiEngine::Collider* other) {

	if (other->GetTag() == "Boss") {
		Dead();
	}
}

void MagicBall::Dead() {
	isLife_ = false;
	phaseRequest_ = Phase::kRoot;

	for (auto& [group, particle] : endEff_) {
		particle->Emit();
		particle->particleData_.emitter_.translate = worldTransform_.GetWorldPos();
	}

	collider_->ColliderOff();
}

void MagicBall::AttackStart(const Vector3& startPos, const Vector3& direction) {

	worldTransform_.translation_ = startPos;
	startPos_ = startPos;

	velocity_ = direction.Normalize() * speed_;

	phaseRequest_ = Phase::kShot;
	collider_->ColliderOn();

	worldTransform_.UpdateMatrix();
}

void MagicBall::RootInit() {
	for (auto& [group, particle] : trailEff_) {
		particle->particleData_.isLoop_ = false;
	}
}

void MagicBall::RootUpdate() {

}

void MagicBall::ShotInit() {
	for (auto& [group, particle] : trailEff_) {
		particle->particleData_.isLoop_ = true;
	}
}

void MagicBall::ShotUpdate() {
	//移動
	worldTransform_.translation_ += velocity_;

	//射程外で消える
	if ((worldTransform_.GetWorldPos() - startPos_).Length() >= firingRange_) {
		Dead();
	}
}