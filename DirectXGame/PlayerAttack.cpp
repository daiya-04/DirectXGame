#include "PlayerAttack.h"


void PlayerAttack::Init(std::shared_ptr<Model> model, const Vector3& startPos, const Vector3& direction) {

	obj_.reset(Object3d::Create(model));

	obj_->worldTransform_.translation_ = startPos;

	velocity_ = direction.Normalize() * speed_;

	collider_.center = GetWorldPos();
	collider_.radius = 0.3f;

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
	collider_.center = GetWorldPos();
}

void PlayerAttack::Draw(const Camera& camera) {

	obj_->Draw(camera);

}

void PlayerAttack::OnCollision() {
	isLife_ = false;
}

Vector3 PlayerAttack::GetWorldPos() const {
	return { obj_->worldTransform_.matWorld_.m[3][0],obj_->worldTransform_.matWorld_.m[3][1] ,obj_->worldTransform_.matWorld_.m[3][2] };
}