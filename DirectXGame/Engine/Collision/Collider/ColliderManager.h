#pragma once
#include "SphereCollider.h"
#include "OBBCollider.h"

#include <vector>
#include <memory>

/// <summary>
/// コライダー管理クラス
/// </summary>
class ColliderManager {
public:

	static ColliderManager* GetInstance();

	static SphereCollider* CreateSphere();
	static OBBCollider* CreateOBB();

	void CheckAllCollision();

	void ColliderClear() { colliders_.clear(); }
	
private:

	SphereCollider* InternalCreateSphere();
	OBBCollider* internalCreateOBB();

	bool CheckCollision(Collider* colliderA, Collider* colliderB);

private:
	//コライダーのvector配列
	std::vector<std::unique_ptr<Collider>> colliders_;


private:

	ColliderManager() = default;
	~ColliderManager() = default;
	ColliderManager(const ColliderManager&) = default;
	ColliderManager& operator=(const ColliderManager&) = default;

};

