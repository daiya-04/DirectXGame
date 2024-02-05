#pragma once

#include "Object3d.h"
#include "ModelManager.h"
#include "WorldTransform.h"
#include "Quaternion.h"
#include "Camera.h"
#include "Hit.h"

class Character
{
public:
	
	virtual void Init(std::vector<Object3d*> models) = 0;
	virtual void Update() = 0;
	virtual void Draw(const Camera& camera) = 0;
	AABB GetAABB() { return collider; }

	void SetColliderSize(Vector3 size) { colliderSize = size; }
	void SetOffset(Vector3 offset) {
		offset_ = offset;
	}

	void ColliderUpdate() {
		collider.min = world_.translation_ - colliderSize + offset_;
		collider.max = world_.translation_ + colliderSize + offset_;

	};
#pragma region
	Vector3 GetPosition()const { return world_.translation_; }
	Vector3 GetColliderSize() { return colliderSize; }
#pragma endregion getter
protected:

	std::vector<Object3d*> models_;
	WorldTransform world_;

	Vector3 colliderSize = {};
	AABB collider;
	Vector3 offset_ = { 0.0f,0.0f,0.0f };
private:



};