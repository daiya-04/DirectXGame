#include "Enemy.h"
#include <cmath>
#include <numbers>

void Enemy::Init(std::vector<uint32_t> modelHandles){

	worldTransform_.Init();
	partsWorldTransform_[Body].Init();
	partsWorldTransform_[Head].Init();

	obj_.resize(modelHandles.size());
	for (size_t index = 0; index < obj_.size(); ++index) {
		obj_[index] = std::make_unique<Object3d>();
		obj_[index].reset(Object3d::Create(modelHandles[index]));
	}

	worldTransform_.scale_ = { 0.5f,0.5f,0.5f };

	partsWorldTransform_[Body].parent_ = &worldTransform_;
	partsWorldTransform_[Head].parent_ = &partsWorldTransform_[Body];
}

void Enemy::Update(){

	worldTransform_.rotation_.y += 5.0f * (std::numbers::pi_v<float> / 180.0f);
	worldTransform_.rotation_.y = std::fmod(worldTransform_.rotation_.y, 2.0f * std::numbers::pi_v<float>);

	//行列更新
	worldTransform_.UpdateMatrix();
	for (size_t index = 0; index < partsWorldTransform_.size(); index++) {
		partsWorldTransform_[index].UpdateMatrix();
	}
}

void Enemy::Draw(const Camera& camera){

	for (size_t index = 0; index < partsWorldTransform_.size(); index++) {
		obj_[index]->Draw(partsWorldTransform_[index], camera);
	}

}
