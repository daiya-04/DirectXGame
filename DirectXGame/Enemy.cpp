#include "Enemy.h"
#include <cmath>
#include <numbers>
#include "GameScene.h"
#include "ModelManager.h"

const WorldTransform* Enemy::target_ = nullptr;

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

	//行列更新
	//worldTransform_.UpdateMatrix();
	Matrix4x4 S = MakeScaleMatrix(worldTransform_.scale_);
	Matrix4x4 T = MakeTranslateMatrix(worldTransform_.translation_);
	worldTransform_.matWorld_ = S * rotateMat_ * T;
	for (size_t index = 0; index < partsWorldTransform_.size(); index++) {
		partsWorldTransform_[index].UpdateMatrix();
	}
}

void Enemy::Update(){

	const float speed = 0.1f;

	//worldTransform_.rotation_.y += 5.0f * (std::numbers::pi_v<float> / 180.0f);
	//worldTransform_.rotation_.y = std::fmod(worldTransform_.rotation_.y, 2.0f * std::numbers::pi_v<float>);
	worldTransform_.translation_.y = 0.0f;

	Vector3 direction = target_->translation_ - worldTransform_.translation_;

	rotateMat_ = DirectionToDirection({ 0.0f,0.0f,1.0f }, direction);

	worldTransform_.translation_ += direction.Normalize() * speed;

	/*if (--attackTimer_ <= 0) {
		attackTimer_ = coolTime_;

		const float bulletSpeed = 0.2f;
		Vector3 shotPos = { worldTransform_.matWorld_.m[3][0],worldTransform_.matWorld_.m[3][1] + 3.4f ,worldTransform_.matWorld_.m[3][2] };
		Vector3 velocity = TransformNormal({ 0.0f,0.0f,1.0f }, rotateMat_).Normalize() * bulletSpeed;

		EnemyBullet* newBullet = new EnemyBullet();
		uint32_t bulletModel = ModelManager::Load("EnemyBullet");
		newBullet->Init(bulletModel, shotPos, velocity);
		gameScene_->AddEnemyBullet(newBullet);

	}*/

	//行列更新
	//worldTransform_.UpdateMatrix();
	Matrix4x4 S = MakeScaleMatrix(worldTransform_.scale_);
	Matrix4x4 T = MakeTranslateMatrix(worldTransform_.translation_);
	worldTransform_.matWorld_ = S * rotateMat_ * T;
	for (size_t index = 0; index < partsWorldTransform_.size(); index++) {
		partsWorldTransform_[index].UpdateMatrix();
	}
}

void Enemy::Draw(const Camera& camera){

	for (size_t index = 0; index < partsWorldTransform_.size(); index++) {
		obj_[index]->Draw(partsWorldTransform_[index], camera);
	}

}

void Enemy::OnCollision() {
	isDead_ = true;

	for (size_t index = 0; index < 50; index++) {

		float range = 0.5f;
		std::uniform_real_distribution<float> distPos(0.0, range);
		std::uniform_real_distribution<float> distVelocityXZ(0.0f, 0.1f);
		std::uniform_real_distribution<float> distVelocityY(0.2f, 0.4f);

		Particle::ParticleData particle;
		particle.worldTransform_.translation_ = { worldTransform_.translation_.x + distPos(randomEngine),worldTransform_.translation_.y + distPos(randomEngine) ,worldTransform_.translation_.z + distPos(randomEngine) };
		particle.velocity_ = { distVelocityXZ(randomEngine), distVelocityY(randomEngine) ,distVelocityXZ(randomEngine) };
		particle.color_ = { 0.0f,0.0f,0.0f,1.0f };
		particle.lifeTime_ = 10.0f;
		particle.currentTime_ = 0.0f;

		gameScene_->AddParticle(particle);
	}

}

void Enemy::SetCoolTime(uint32_t coolTime) {

	coolTime_ = 60 * coolTime;
	attackTimer_ = coolTime_;

}

Vector3 Enemy::GetWorldPos() const {

	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1] + size_.y;
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;

}
