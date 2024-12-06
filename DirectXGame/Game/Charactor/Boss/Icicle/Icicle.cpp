#include "Icicle.h"

#include "Easing.h"
#include "TextureManager.h"
#include "ParticleManager.h"


void Icicle::Init(const std::shared_ptr<Model>& model) {

	obj_.reset(Object3d::Create(model));

	collider_.radius = 1.0f;


	///エフェクト設定
	/*particle_.reset(GPUParticle::Create(TextureManager::Load("mist.png"),5000));

	particle_->particleData_.emitter_.color = Vector4(0.05f, 0.94f, 0.85f, 1.0f);
	particle_->particleData_.emitter_.emitterType = GPUParticle::EmitShape::Sphere;
	particle_->particleData_.isLoop_ = false;

	particle_->particleData_.overLifeTime_.startColor = Vector3(0.05f, 0.94f, 0.85f);
	particle_->particleData_.overLifeTime_.endColor = Vector3(1.0f, 1.0f, 1.0f);

	particle_->particleData_.overLifeTime_.isAlpha = 1;
	particle_->particleData_.overLifeTime_.midAlpha = 1.0f;

	particle_->particleData_.overLifeTime_.endScale = 0.3f;*/

	/*iceSpark_.reset(GPUParticle::Create(TextureManager::Load("circle.png"), 1024));
	iceSpark_->SetParticleData(ParticleManager::Load("IceSpark_1"));

	coolAir_.reset(GPUParticle::Create(TextureManager::Load("circle.png"), 1024));
	coolAir_->SetParticleData(ParticleManager::Load("IceCoolAir"));

	iceCreate_.reset(GPUParticle::Create(TextureManager::Load("circle.png"), 10000));
	iceCreate_->SetParticleData(ParticleManager::Load("IcicleCreate"));*/
	//iceCreate_->particleData_.isLoop_ = false;

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

	rotateMat_ = DirectionToDirection({ 0.0f,0.0f,1.0f }, direction_);
	
	obj_->worldTransform_.UpdateMatrixRotate(rotateMat_);
	/*particle_->particleData_.emitter_.translate = obj_->GetWorldPos();

	particle_->Update();
	iceSpark_->Update();
	coolAir_->Update();
	iceCreate_->Update();*/
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

	ColliderUpdate();
}

void Icicle::Draw(const Camera& camera) {
	obj_->Draw(camera);
}

void Icicle::DrawParticle(const Camera& camera) {
	/*particle_->Draw(camera);
	iceSpark_->Draw(camera);
	coolAir_->Draw(camera);
	iceCreate_->Draw(camera);*/
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

void Icicle::OnCollision() {
	phaseRequest_ = Phase::kRoot;
	isLife_ = false;

	//ヒットエフェクト開始
	/*iceSpark_->particleData_.emitter_.emit = 1;
	coolAir_->particleData_.emitter_.emit = 1;
	iceSpark_->particleData_.emitter_.translate = obj_->GetWorldPos();
	coolAir_->particleData_.emitter_.translate = obj_->GetWorldPos();

	particle_->particleData_.isLoop_ = false;*/
	for (auto& [group, particle] : trailEff_) {
		particle->particleData_.isLoop_ = false;
	}

	for (auto& [group, particle] : hitEffect_) {
		particle->Emit();
		particle->particleData_.emitter_.translate = obj_->GetWorldPos();
	}

}

void Icicle::AttackStart() {
	phaseRequest_ = Phase::kCreate;
	isLife_ = true;
}

void Icicle::SetAttackData(const Vector3& pos, const Vector3& direction, float interval) {

	obj_->worldTransform_.translation_ = pos;
	direction_ = direction;
	rotateMat_ = DirectionToDirection({ 0.0f,0.0f,1.0f }, direction_);
	waitData_.waitTime_ = static_cast<int32_t>(60.0f * interval);

	obj_->worldTransform_.UpdateMatrixRotate(rotateMat_);
}

void Icicle::RootInit() {

	obj_->worldTransform_.scale_ = {};
	/*particle_->particleData_.emitter_.count = 0;
	particle_->particleData_.overLifeTime_.isColor = 0;
	particle_->particleData_.overLifeTime_.isScale = 0;*/

}

void Icicle::RootUpdate() {

	//particle_->particleData_.emitter_.frequencyTime = 0.0f;

}

void Icicle::CreateInit() {

	createData_.param_ = 0.0f;

	/*iceCreate_->particleData_.isLoop_ = true;
	iceCreate_->particleData_.emitter_.translate = obj_->GetWorldPos();*/
	for (auto& [group, particle] : createEffect_) {
		particle->particleData_.isLoop_ = true;
		particle->particleData_.emitter_.translate = obj_->GetWorldPos();
	}
}

void Icicle::CreateUpdate() {

	createData_.param_ += 0.01f;
	createData_.param_ = std::clamp(createData_.param_, 0.0f, 1.0f);

	float T = Easing::easeInSine(createData_.param_);
	//少しずつ大きくする
	obj_->worldTransform_.scale_ = Lerp(T, Vector3(), Vector3(0.5f, 0.5f, 0.5f));

	if (createData_.param_ >= 1.0f) {
		phaseRequest_ = Phase::kWait;
		//iceCreate_->particleData_.isLoop_ = false;
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

	/*particle_->particleData_.emitter_.count = 100;
	particle_->particleData_.emitter_.frequency = 1.0f / 60.0f;
	particle_->particleData_.emitter_.lifeTime = 1.0f;
	particle_->particleData_.emitter_.scale = 0.3f;
	particle_->particleData_.emitter_.radius = 0.5f;
	particle_->particleData_.emitter_.speed = 0.3f;

	particle_->particleData_.overLifeTime_.isColor = 1;
	particle_->particleData_.overLifeTime_.isScale = 1;
	particle_->particleData_.isLoop_ = true;*/
	for (auto& [group, particle] : trailEff_) {
		particle->particleData_.isLoop_ = true;
	}


}

void Icicle::ShotUpdate() {
	//つららとターゲットの向き計算
	Vector3 diff = target_->translation_ - obj_->worldTransform_.translation_;
	//距離計算
	float distance = diff.Length();
	//一定距離まで追尾
	if (distance >= shotData_.trackingDist_) {
		targetDict_ = (target_->translation_ - obj_->worldTransform_.translation_);
	}
	
	shotData_.param_ += 0.005f;

	//少しずつプレイヤーの方向に向かせる
	direction_ = Lerp(shotData_.param_, direction_.Normalize(), targetDict_.Normalize());

	velocity_ = direction_ * speed_;

	obj_->worldTransform_.translation_ += velocity_;
	//地面より下にはいかない
	if (obj_->worldTransform_.translation_.y <= 0.0f) {
		OnCollision();
	}

}

