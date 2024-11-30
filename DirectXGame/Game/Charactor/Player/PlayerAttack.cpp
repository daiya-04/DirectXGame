#include "PlayerAttack.h"

#include "TextureManager.h"


void PlayerAttack::Init(std::shared_ptr<Model> model, const Vector3& startPos, const Vector3& direction) {

	obj_.reset(Object3d::Create(model));

	obj_->worldTransform_.translation_ = startPos;
	startPos_ = startPos;

	velocity_ = direction.Normalize() * speed_;

	collider_.center = GetWorldPos();
	collider_.radius = 0.3f;

	///エフェクト設定

	particle_.reset(GPUParticle::Create(TextureManager::Load("particle.png"), 10000));

	particle_->particleData_.emitter_.radius = 0.25f;
	particle_->particleData_.emitter_.scale = 0.05f;
	particle_->particleData_.emitter_.frequency = 1.0f / 60.0f;
	particle_->particleData_.emitter_.count = 500;
	particle_->particleData_.emitter_.speed = 0.0f;
	particle_->particleData_.emitter_.lifeTime = 0.7f;
	particle_->particleData_.emitter_.color = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
	particle_->particleData_.emitter_.emitterType = GPUParticle::EmitShape::Sphere;

	particle_->particleData_.overLifeTime_.isAlpha = 1;
	particle_->particleData_.overLifeTime_.midAlpha = 1.0f;

	particle_->particleData_.overLifeTime_.isScale = 1;
	particle_->particleData_.overLifeTime_.startScale = 0.1f;
	particle_->particleData_.overLifeTime_.endScale = 0.0f;

	///

}

void PlayerAttack::Update() {
	//移動
	obj_->worldTransform_.translation_ += velocity_;

	obj_->worldTransform_.UpdateMatrix();
	particle_->particleData_.emitter_.translate = GetWorldPos();
	collider_.center = GetWorldPos();
	
	particle_->Update();

	//射程外で消える
	if ((GetWorldPos() - startPos_).Length() >= firingRange_) {
		isLife_ = false;
	}

}

void PlayerAttack::Draw(const Camera& camera) {
	obj_->Draw(camera);
}

void PlayerAttack::DrawParticle(const Camera& camera) {
	particle_->Draw(camera);
}

void PlayerAttack::OnCollision() {
	isLife_ = false;
}

Vector3 PlayerAttack::GetWorldPos() const {
	return { obj_->worldTransform_.matWorld_.m[3][0],obj_->worldTransform_.matWorld_.m[3][1] ,obj_->worldTransform_.matWorld_.m[3][2] };
}