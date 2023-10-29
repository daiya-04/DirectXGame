#include "Goal.h"
#include <cassert>

void Goal::Initialize(Object3d* model, const Vector3& position) {
	assert(model);
	model_ = model;

	worldTransform_.translation_ = position;
	worldTransform_.translation_.y += worldTransform_.scale_.y;

}

void Goal::Update() {


	worldTransform_.UpdateMatrix();
}

void Goal::Draw(const ViewProjection& viewProjection) {

	model_->Draw(worldTransform_, viewProjection);

}

Vector3 Goal::GetWorldPos() const {
	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
