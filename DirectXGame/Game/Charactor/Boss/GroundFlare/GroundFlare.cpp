#include "GroundFlare.h"

#include "Easing.h"
#include "TextureManager.h"
#include "ParticleManager.h"
#include "ColliderManager.h"
#include "ModelManager.h"

size_t GroundFlare::heightAdjustmentIndex_ = 1;

GroundFlare::~GroundFlare() {
	DaiEngine::ColliderManager::GetInstance()->RemoveCollider(collider_.get());
}

void GroundFlare::Init() {

	worldTransform_.Init();

	std::shared_ptr<DaiEngine::Model> model = DaiEngine::ModelManager::LoadOBJ("WarningZone");
	warningZone_.reset(DaiEngine::Object3d::Create(model));

	collider_ = std::make_unique<DaiEngine::OBBCollider>();
	collider_->Init("BossAttack", worldTransform_, { 1.0f,3.0f,1.0f });
	collider_->SetPosition(Vector3(0.0, collider_->GetSize().y, 0.0f));
	collider_->SetStayCallback([this](DaiEngine::Collider* other) {this->OnCollision(other); });
	collider_->SetValue("Damage", damage_);
	collider_->ColliderOff();
	DaiEngine::ColliderManager::GetInstance()->AddCollider(collider_.get());

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

	worldTransform_.UpdateMatrix();
	warningZone_->worldTransform_.UpdateMatrix();
	collider_->Update(MakeIdentity44());
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

void GroundFlare::OnCollision(DaiEngine::Collider* other) {

	if (other->GetTag() == "Player") {
		collider_->ColliderOff();
	}

}

void GroundFlare::AttackStart(const Vector3& firePos, int32_t interval) {

	isAttack_ = true;
	phaseRequest_ = Phase::kWait;

	worldTransform_.translation_ = firePos;
	warningZone_->worldTransform_.translation_ = firePos;
	
	workWait_.waitTime_ = interval;

	for (auto& [group, particle] : fire_) {
		particle->particleData_.emitter_.translate = firePos;
	}

}

void GroundFlare::RootInit() {

}

void GroundFlare::RootUpdate() {

}

void GroundFlare::WaitInit() {

	workWait_.param_ = workWait_.waitTime_;

}

void GroundFlare::WaitUpdate() {

	if (--workWait_.param_ <= 0) {
		phaseRequest_ = Phase::kWarning;
	}

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
	collider_->ColliderOn();

	burnScar_->EffectStart(worldTransform_.GetWorldPos());
	burnScar_->HeightAdjustment(heightAdjustmentIndex_ * 0.001f);
	heightAdjustmentIndex_ = (heightAdjustmentIndex_ % 6) + 1;

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
		collider_->ColliderOff();
	}

}
