#include "Enemy.h"
#include <cmath>
#include <numbers>

void Enemy::Initialize(const std::vector<Object3d*>& models){

	models_ = models;

	InitializeFloatingGimmick();

	worldTransform_.scale_ = { 0.5f,0.5f,0.5f };
	partsWorldTransform_[Body].parent_ = &worldTransform_;
	partsWorldTransform_[Head].parent_ = &partsWorldTransform_[Body];

}

void Enemy::Update(){

	Vector3 prePos = worldTransform_.translation_;
	theta_ += 0.01f;
	theta_ = std::fmod(theta_, 2.0f * (float)std::numbers::pi);

	Vector3 distance = { center_.x - radius_,0.0f,0.0f };

	Vector3 move = {
		distance.x * std::cosf(theta_) - distance.z * std::sinf(theta_),
		0.0f,
		distance.x * std::sinf(theta_) + distance.z * std::cosf(theta_),
	};

	worldTransform_.translation_ = move + center_;

	Vector3 rotate = worldTransform_.translation_ - prePos;

	worldTransform_.rotation_.y = std::atan2(rotate.x, rotate.z);

	UpdateFloatingGimmick();


	//行列更新
	worldTransform_.UpdateMatrix();
	for (size_t index = 0; index < partsWorldTransform_.size(); index++) {
		partsWorldTransform_[index].UpdateMatrix();
	}
}

void Enemy::Draw(const ViewProjection& viewProjection){

	for (size_t index = 0; index < partsWorldTransform_.size(); index++) {
		models_[index]->Draw(partsWorldTransform_[index], viewProjection);
	}

}

void Enemy::InitializeFloatingGimmick() {

	floatingParameter_ = 0.0f;

}

void Enemy::UpdateFloatingGimmick() {

	//1フレームでのパラメータ加算値
	const float step = 2.0f * (float)std::numbers::pi / (float)cycle;

	floatingParameter_ += step;

	floatingParameter_ = std::fmod(floatingParameter_, 2.0f * (float)std::numbers::pi);

	partsWorldTransform_[Head].translation_.y = std::sinf(floatingParameter_) * amplitude;

}

Vector3 Enemy::GetWorldPos() const {
	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1] + size_.y;
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
