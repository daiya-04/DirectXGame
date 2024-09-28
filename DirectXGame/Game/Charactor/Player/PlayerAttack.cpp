#include "PlayerAttack.h"

#include "TextureManager.h"


void PlayerAttack::Init(std::shared_ptr<Model> model, const Vector3& startPos, const Vector3& direction) {

	obj_.reset(Object3d::Create(model));

	obj_->worldTransform_.translation_ = startPos;
	startPos_ = startPos;

	velocity_ = direction.Normalize() * speed_;

	collider_.center = GetWorldPos();
	collider_.radius = 0.3f;

	particle_.reset(GPUParticle::Create(TextureManager::Load("particle.png"), 10000));

	particle_->emitter_.size = Vector3(0.25f, 0.25f, 0.25f);
	particle_->emitter_.scale = 0.05f;
	particle_->emitter_.angle = 360.0f;
	particle_->emitter_.frequency = 1.0f / 60.0f;
	particle_->emitter_.count = 500;
	particle_->emitter_.speed = 0.0f;
	particle_->emitter_.lifeTime = 0.7f;
	particle_->emitter_.color = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
	particle_->emitter_.emitterType = 0;

}

void PlayerAttack::Update() {

	obj_->worldTransform_.translation_ += velocity_;

	

	if (--lifeCount_ <= 0) {
		obj_->worldTransform_.scale_ -= Vector3(0.1f, 0.1f, 0.1f);
	}

	lifeCount_ = max(lifeCount_, 0);

	if (obj_->worldTransform_.scale_.x <= 0.0f) {
		isLife_ = false;
	}

	particle_->emitter_.direction = -velocity_.Normalize();

	obj_->worldTransform_.UpdateMatrix();
	particle_->emitter_.translate = GetWorldPos();
	collider_.center = GetWorldPos();
	
	particle_->Update();

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