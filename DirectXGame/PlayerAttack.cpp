#include "PlayerAttack.h"


void PlayerAttack::Init(std::shared_ptr<Model> model, const Vector3& startPos, const Vector3& direction) {

	obj_.reset(Object3d::Create(model));

	obj_->worldTransform_.translation_ = startPos;

	velocity_ = direction.Normalize() * speed_;

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

	obj_->worldTransform_.UpdateMatrix();
}

void PlayerAttack::Draw(const Camera& camera) {

	obj_->Draw(camera);

}

void PlayerAttack::OnCollision() {
	isLife_ = false;
}