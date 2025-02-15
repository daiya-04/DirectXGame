#include "PlayerMagicBall.h"

#include "TextureManager.h"
#include "ColliderManager.h"
#include "ParticleManager.h"

PlayerMagicBall::~PlayerMagicBall() {
	DaiEngine::ColliderManager::GetInstance()->RemoveCollider(collider_.get());
}

void PlayerMagicBall::Init(std::shared_ptr<DaiEngine::Model> model) {

	obj_.reset(DaiEngine::Object3d::Create(model));

	collider_ = std::make_unique<DaiEngine::SphereCollider>();
	collider_->Init("PlayerAttack", obj_->worldTransform_, 0.3f);
	collider_->SetCallbackFunc([this](DaiEngine::Collider* other) {this->OnCollision(other); });
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

void PlayerMagicBall::Update() {

	//フェーズ切り替えの初期化
	if (phaseRequest_) {
		phase_ = phaseRequest_.value();

		phaseInitTable_[phase_]();

		phaseRequest_ = std::nullopt;
	}
	//フェーズ更新
	phaseUpdateTable_[phase_]();


	//行列計算
	obj_->worldTransform_.UpdateMatrix();
	collider_->Update();

	//エフェクト更新
	for (auto& [group, particle] : trailEff_) {
		particle->particleData_.emitter_.translate = GetWorldPos();
		particle->Update();
	}
	for (auto& [group, particle] : endEff_) {
		particle->Update();
	}

}

void PlayerMagicBall::Draw(const DaiEngine::Camera& camera) {
	if (phase_ == Phase::kRoot) { return; }
	obj_->Draw(camera);
}

void PlayerMagicBall::DrawParticle(const DaiEngine::Camera& camera) {
	for (auto& [group, particle] : trailEff_) {
		particle->Draw(camera);
	}
	for (auto& [group, particle] : endEff_) {
		particle->Draw(camera);
	}
}

void PlayerMagicBall::OnCollision([[maybe_unused]] DaiEngine::Collider* other) {

	if (other->GetTag() == "Boss") {
		Dead();
	}
}

void PlayerMagicBall::Dead() {
	isLife_ = false;
	phaseRequest_ = Phase::kRoot;

	for (auto& [group, particle] : endEff_) {
		particle->Emit();
		particle->particleData_.emitter_.translate = obj_->GetWorldPos();
	}

	collider_->ColliderOff();
}

void PlayerMagicBall::StartAttack(const Vector3& startPos, const Vector3& direction) {

	obj_->worldTransform_.translation_ = startPos;
	startPos_ = startPos;

	velocity_ = direction.Normalize() * speed_;

	phaseRequest_ = Phase::kShot;
	collider_->ColliderOn();

	obj_->worldTransform_.UpdateMatrix();
}

Vector3 PlayerMagicBall::GetWorldPos() const {
	return { obj_->worldTransform_.matWorld_.m[3][0],obj_->worldTransform_.matWorld_.m[3][1] ,obj_->worldTransform_.matWorld_.m[3][2] };
}

void PlayerMagicBall::RootInit() {
	for (auto& [group, particle] : trailEff_) {
		particle->particleData_.isLoop_ = false;
	}
}

void PlayerMagicBall::RootUpdate() {

}

void PlayerMagicBall::ShotInit() {
	for (auto& [group, particle] : trailEff_) {
		particle->particleData_.isLoop_ = true;
	}
}

void PlayerMagicBall::ShotUpdate() {
	//移動
	obj_->worldTransform_.translation_ += velocity_;

	//射程外で消える
	if ((GetWorldPos() - startPos_).Length() >= firingRange_) {
		Dead();
	}
}