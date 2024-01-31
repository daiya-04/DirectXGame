#pragma once
#include "Object3d.h"
#include "WorldTransform.h"
#include "Camera.h"
#include "Vec3.h"
#include <memory>

class EnemyBullet{

	std::unique_ptr<Object3d> obj_;
	WorldTransform worldTransform_;
	Vector3 size_ = { 0.3f,0.3f,0.3f };

	Vector3 velocity_{};

	bool isDead_ = false;

	static const uint32_t kLifeTime_ = 60 * 10;
	uint32_t lifeTimer_ = kLifeTime_;

public:

	void Init(uint32_t modelHandle, const Vector3& pos, const Vector3& velocity);

	void Update();

	void Draw(const Camera& camera);

	void OnCollision();

	bool IsDead() const { return isDead_; }

	const WorldTransform& GetWorldtransform() { return worldTransform_; }
	Vector3 GetWorldPos() const;
	Vector3 GetSize() const { return size_; }

};

