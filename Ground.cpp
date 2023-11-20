#include "Ground.h"
#include <cassert>
#include <cmath>
#include <numbers>
#include "ImGuiManager.h"

void Ground::Initialize(Object3d* model, Type type, const Vector3& position, const Vector3& scale) {

	assert(model);
	model_ = model;
	type_ = type;
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = scale;
	size_ = { worldTransform_.scale_.x * 6.0f,0.0f,worldTransform_.scale_.z * 6.0f };

	worldTransform_.UpdateMatrix();
}

void Ground::Update() {

	if (type_ == Type::Dynamic) {

		const float step = 2.0f * (float)std::numbers::pi / (float)cycle;

		moveingParameter += step;
		moveingParameter = std::fmod(moveingParameter, 2.0f * (float) std::numbers::pi);

		velocity_.x = std::cosf(moveingParameter) * amplitude;
				
		worldTransform_.translation_ += velocity_;

		

	}


	size_ = { worldTransform_.scale_.x * 6.0f,0.0f,worldTransform_.scale_.z * 6.0f };
	worldTransform_.UpdateMatrix();
}

void Ground::Draw(const ViewProjection& viewProjection) {

	model_->Draw(worldTransform_,viewProjection);

}

Vector3 Ground::GetWorldPos() const {
	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
