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
	void ColliderUpdate() {
		collider.min.x = world_.translation_.x - colliderSize.x;
		collider.min.y = world_.translation_.y - colliderSize.y;
		collider.min.z = world_.translation_.z - colliderSize.z;
		
		collider.max.x = world_.translation_.x + colliderSize.x;
		collider.max.y = world_.translation_.y + colliderSize.y;
		collider.max.z = world_.translation_.z + colliderSize.z;
	};
#pragma region
	Vector3 GetPosition()const { return world_.translation_; }
#pragma endregion getter
protected:

	std::vector<Object3d*> models_;
	WorldTransform world_;

	Vector3 colliderSize = {};
	AABB collider;

private:



};