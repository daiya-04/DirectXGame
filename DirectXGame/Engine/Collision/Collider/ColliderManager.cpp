#include "ColliderManager.h"
#include "Hit.h"

namespace DaiEngine {
	ColliderManager* ColliderManager::GetInstance() {
		static ColliderManager instance;

		return &instance;
	}

	void ColliderManager::RemoveCollider(Collider* collider) {
		if (colliders_.size() == 0) { return; }
		colliders_.erase(std::remove(colliders_.begin(), colliders_.end(), collider), colliders_.end());
	}

	void ColliderManager::CheckAllCollision() {

		for (size_t i = 0; i < colliders_.size(); i++) {
			Collider* colliderA = colliders_[i];
			if (colliderA == nullptr || !colliderA->IsHit()) { continue; }

			for (size_t j = 0; j < colliders_.size(); j++) {

				Collider* colliderB = colliders_[j];
				if (colliderB == nullptr || !colliderB->IsHit()) { continue; }

				if (CheckCollision(colliderA, colliderB)) {

					colliderA->OnCollision(colliderB);
					colliderB->OnCollision(colliderA);

				}

			}
		}

	}

	bool ColliderManager::CheckCollision(Collider* colliderA, Collider* colliderB) {
		//形状タイプ取得
		const auto& typeA = colliderA->GetType();
		const auto& typeB = colliderB->GetType();

		//形状データ取得
		const auto& shapeA = colliderA->GetShape();
		const auto& shapeB = colliderB->GetShape();


		if (typeA == Collider::Type::Sphere && typeB == Collider::Type::Sphere) {
			return IsCollision(std::get<Shapes::Sphere>(shapeA), std::get<Shapes::Sphere>(shapeB));
		}
		else if (typeA == Collider::Type::OBB && typeB == Collider::Type::OBB) {
			return IsCollision(std::get<Shapes::OBB>(shapeA), std::get<Shapes::OBB>(shapeB));
		}
		else if (typeA == Collider::Type::Sphere && typeB == Collider::Type::OBB) {
			return IsCollision(std::get<Shapes::OBB>(shapeB), std::get<Shapes::Sphere>(shapeA));
		}
		else if (typeA == Collider::Type::OBB && typeB == Collider::Type::Sphere) {
			return IsCollision(std::get<Shapes::OBB>(shapeA), std::get<Shapes::Sphere>(shapeB));
		}


		return false;
	}

}
