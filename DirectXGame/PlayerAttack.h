#pragma once
#include "WorldTransform.h"
#include "Camera.h"
#include "Object3d.h"
#include "Vec3.h"
#include "CollisionShapes.h"
#include <memory>

class PlayerAttack {
public:

	void Init(std::shared_ptr<Model> model, const Vector3& startPos, const Vector3& direction);

	void Update();

	void Draw(const Camera& camera);

	void OnCollision();

	bool IsLife() const { return isLife_; }

	Shapes::Sphere GetCollider() const { return collider_; }

	Vector3 GetWorldPos() const;

private:

	std::unique_ptr<Object3d> obj_;
	Shapes::Sphere collider_;

	float speed_ = 0.3f;
	Vector3 velocity_{};

	int32_t lifeTime_ = 60 * 2;
	int32_t lifeCount_ = lifeTime_;

	bool isLife_ = true;


};

