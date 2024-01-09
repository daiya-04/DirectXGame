#include "EnemyBullet.h"

void EnemyBullet::Init(uint32_t modelHandle, const Vector3& pos, const Vector3& velocity){

	worldTransform_.Init();

	obj_ = std::make_unique<Object3d>();
	obj_.reset(Object3d::Create(modelHandle));

	worldTransform_.translation_ = pos;
	velocity_ = velocity;

}

void EnemyBullet::Update(){

	worldTransform_.translation_ += velocity_;

	

	if (--lifeTimer_ <= 0) {
		isDead_ = true;
	}

	//行列更新
	worldTransform_.UpdateMatrix();
}

void EnemyBullet::Draw(const Camera& camera){

	obj_->Draw(worldTransform_, camera);

}

void EnemyBullet::OnCollision(){
	isDead_ = true;
}

Vector3 EnemyBullet::GetWorldPos() const{

	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
