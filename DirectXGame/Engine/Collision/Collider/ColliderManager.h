#pragma once
#include "SphereCollider.h"
#include "OBBCollider.h"

#include <vector>
#include <memory>

/// <summary>
/// コライダー管理クラス
/// </summary>

namespace DaiEngine {
	class ColliderManager {
	public:

		static ColliderManager* GetInstance();

		void AddCollider(Collider* collider) { colliders_.push_back(collider); }

		void RemoveCollider(Collider* collider);

		void CheckAllCollision();

	private:

		bool CheckCollision(Collider* colliderA, Collider* colliderB);

	private:
		//コライダーのvector配列
		std::vector<Collider*> colliders_;


	private:

		ColliderManager() = default;
		~ColliderManager() = default;
		ColliderManager(const ColliderManager&) = default;
		ColliderManager& operator=(const ColliderManager&) = default;

	};
}
