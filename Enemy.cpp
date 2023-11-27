#include "Enemy.h"
#include "ModelManager.h"
#include "Easing.h"
#include <cmath>
#include <numbers>

void (Enemy::* Enemy::BehaviorTable[])() = {
	&Enemy::RootUpdate,
	&Enemy::DeadUpdate,
};

void Enemy::Initialize(std::vector<uint32_t> modelHandles){

	modelHandles_ = modelHandles;

	objects_.resize(modelHandles.size());
	for (size_t index = 0; index < objects_.size(); index++) {
		objects_[index] = std::make_unique<Object3d>();
		objects_[index]->Initialize(modelHandles[index]);
	}

	InitializeFloatingGimmick();

	worldTransform_.scale_ = { 0.5f,0.5f,0.5f };
	partsWorldTransform_[Body].parent_ = &worldTransform_;
	partsWorldTransform_[Head].parent_ = &partsWorldTransform_[Body];

}

void Enemy::Update(){

	

	if (behaviorRequest_) {

		behavior_ = behaviorRequest_.value();

		switch (behavior_) {
		case Behavior::kRoot:
		default:
			RootInitialize();
			break;
		case Behavior::kDead:
			DeadInitialize();
			break;
		}

		behaviorRequest_ = std::nullopt;
	}

	(this->*BehaviorTable[static_cast<size_t>(behavior_)])();

	
	for (const auto& modelHandle : modelHandles_) {
		ModelManager::GetInstance()->SetColor(modelHandle, color_);
	}
	//行列更新
	worldTransform_.UpdateMatrix();
	for (size_t index = 0; index < partsWorldTransform_.size(); index++) {
		partsWorldTransform_[index].UpdateMatrix();
	}
}

void Enemy::Draw(const ViewProjection& viewProjection){

	if (isDead_) { return; }
	for (size_t index = 0; index < partsWorldTransform_.size(); index++) {
		objects_[index]->Draw(partsWorldTransform_[index], viewProjection);
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

void Enemy::OnCollision(){
	life_--;

}

void Enemy::Reset() {



}

Vector3 Enemy::GetWorldPos() const {
	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1] + size_.y;
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void Enemy::RootInitialize(){

	worldTransform_.rotation_.y = 0.0f;

}

void Enemy::RootUpdate(){

	worldTransform_.rotation_.y += 5.0f * (std::numbers::pi_v<float> / 180.0f);
	worldTransform_.rotation_.y = std::fmod(worldTransform_.rotation_.y, 2.0f * (float)std::numbers::pi);

	UpdateFloatingGimmick();

	if (life_ <= 0) {
		behaviorRequest_ = Behavior::kDead;
	}

}

void Enemy::DeadInitialize(){

	crushParam_ = 0.0f;

}

void Enemy::DeadUpdate(){

	crushParam_ += 0.05f;
	float T = Easing::easeInSine(crushParam_);
	worldTransform_.scale_.y = Lerp(T, worldTransform_.scale_.y, 0.1f);

	if (crushParam_ >= 1.0f) {
		isDead_ = true;
	}

}
