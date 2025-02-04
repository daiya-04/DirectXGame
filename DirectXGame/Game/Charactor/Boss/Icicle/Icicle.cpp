#include "Icicle.h"

#include "Easing.h"
#include "TextureManager.h"
#include "ParticleManager.h"
#include "ColliderManager.h"

Icicle::~Icicle() {
	DaiEngine::ColliderManager::GetInstance()->RemoveCollider(collider_.get());
}

void Icicle::Init(const std::shared_ptr<DaiEngine::Model>& model) {

	obj_.reset(DaiEngine::Object3d::Create(model));

	collider_ = std::make_unique<DaiEngine::SphereCollider>();
	collider_->Init("BossAttack", obj_->worldTransform_, 1.0f);
	DaiEngine::ColliderManager::GetInstance()->AddCollider(collider_.get());


	///エフェクト設定
	createEffect_ = ParticleManager::Load("IcicleCreate");
	hitEffect_ = ParticleManager::Load("IcicleImpact");
	trailEff_ = ParticleManager::Load("IcicleTrail");

	for (auto& [group, particle] : createEffect_) {
		particle->particleData_.isLoop_ = false;
	}
	for (auto& [group, particle] : trailEff_) {
		particle->particleData_.isLoop_ = false;
	}
	
	///

	isLife_ = false;
	preIsLife_ = false;

}

void Icicle::Update() {

	preIsLife_ = isLife_;
	//フェーズ切り替えの初期化
	if (phaseRequest_) {

		phase_ = phaseRequest_.value();

		phaseInitTable_[phase_]();

		phaseRequest_ = std::nullopt;

	}
	//フェーズ更新
	phaseUpdateTable_[phase_]();
	//回転行列計算
	rotateMat_ = DirectionToDirection({ 0.0f,0.0f,1.0f }, direction_);
	//行列更新
	obj_->worldTransform_.UpdateMatrixRotate(rotateMat_);
	
	for (auto& [group, particle] : createEffect_) {
		particle->Update();
	}
	for (auto& [group, particle] : hitEffect_) {
		particle->Update();
	}
	for (auto& [group, particle] : trailEff_) {
		particle->particleData_.emitter_.translate = obj_->GetWorldPos();
		particle->Update();
	}

	collider_->Update();
}

void Icicle::Draw(const DaiEngine::Camera& camera) {
	obj_->Draw(camera);
}

void Icicle::DrawParticle(const DaiEngine::Camera& camera) {
	for (auto& [group, particle] : createEffect_) {
		particle->Draw(camera, true);
	}
	for (auto& [group, particle] : hitEffect_) {
		particle->Draw(camera);
	}
	for (auto& [group, particle] : trailEff_) {
		particle->Draw(camera, true);
	}

}

void Icicle::OnCollision([[maybe_unused]] DaiEngine::Collider* other) {
	phaseRequest_ = Phase::kRoot;
	isLife_ = false;

	//ヒットエフェクト開始
	for (auto& [group, particle] : trailEff_) {
		particle->particleData_.isLoop_ = false;
	}

	for (auto& [group, particle] : hitEffect_) {
		particle->Emit();
		particle->particleData_.emitter_.translate = obj_->GetWorldPos();
	}

	collider_->ColliderOff();

}

void Icicle::AttackStart() {
	phaseRequest_ = Phase::kCreate;
	isLife_ = true;
}

void Icicle::SetAttackData(const Vector3& pos, const Vector3& direction, float interval) {

	obj_->worldTransform_.translation_ = pos;
	direction_ = direction;
	rotateMat_ = DirectionToDirection({ 0.0f,0.0f,1.0f }, direction_);
	const float kFramePerSecond = 60.0f;
	waitData_.waitTime_ = static_cast<int32_t>(kFramePerSecond * interval);

	obj_->worldTransform_.UpdateMatrixRotate(rotateMat_);
}

void Icicle::RootInit() {

	obj_->worldTransform_.scale_ = {};

}

void Icicle::RootUpdate() {



}

void Icicle::CreateInit() {

	createData_.param_ = 0.0f;

	for (auto& [group, particle] : createEffect_) {
		particle->particleData_.isLoop_ = true;
		particle->particleData_.emitter_.translate = obj_->GetWorldPos();
	}
}

void Icicle::CreateUpdate() {

	createData_.param_ += createData_.speed_;
	createData_.param_ = std::clamp(createData_.param_, 0.0f, 1.0f);

	float T = Easing::easeInSine(createData_.param_);
	//少しずつ大きくする
	obj_->worldTransform_.scale_ = Lerp(T, createData_.startScale_, createData_.endScale_);

	if (createData_.param_ >= 1.0f) {
		phaseRequest_ = Phase::kWait;
		for (auto& [group, particle] : createEffect_) {
			particle->particleData_.isLoop_ = false;
		}
	}

}

void Icicle::WaitInit() {

	waitData_.count_ = 0;

}

void Icicle::WaitUpdate() {

	waitData_.count_++;
	waitData_.count_ = std::clamp(waitData_.count_, 0, waitData_.waitTime_);

	if (waitData_.count_ >= waitData_.waitTime_) {
		phaseRequest_ = Phase::kShot;
	}

}

void Icicle::ShotInit() {

	shotData_.param_ = 0.0f;

	for (auto& [group, particle] : trailEff_) {
		particle->particleData_.isLoop_ = true;
	}

	collider_->ColliderOn();

}

void Icicle::ShotUpdate() {
	//つららとターゲットの向き計算
	Vector3 diff = *target_ - obj_->worldTransform_.translation_;
	//距離計算
	float distance = diff.Length();
	//一定距離まで追尾
	if (distance >= shotData_.trackingDist_) {
		targetDict_ = *target_ - obj_->worldTransform_.translation_;
	}
	
	shotData_.param_ += shotData_.rotateSpeed_;

	//少しずつプレイヤーの方向に向かせる
	direction_ = Lerp(shotData_.param_, direction_.Normalize(), targetDict_.Normalize());
	//速度計算
	velocity_ = direction_ * speed_;

	obj_->worldTransform_.translation_ += velocity_;
	//地面より下にはいかない
	/*if (obj_->worldTransform_.translation_.y <= 0.0f) {
		OnCollision();
	}*/

}

