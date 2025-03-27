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

			for (size_t j = i + 1; j < colliders_.size(); j++) {

				Collider* colliderB = colliders_[j];
				if (colliderB == nullptr || !colliderB->IsHit()) { continue; }

				//衝突しているか？
				bool isCollision = CheckCollision(colliderA, colliderB);

				//衝突ペアの順序を統一
				auto key = std::minmax(colliderA, colliderB);
				//衝突ペアの状態を取得
				auto& state = collisionMap_[key];

				//衝突状態に合わせた関数を呼び出す
				if (isCollision && !state.preCollision) {
					//衝突した瞬間
					colliderA->OnCollision(Collider::CollisionPhase::Enter, colliderB);
					colliderB->OnCollision(Collider::CollisionPhase::Enter, colliderA);

				}else if (isCollision && state.preCollision) {
					//衝突中
					colliderA->OnCollision(Collider::CollisionPhase::Stay, colliderB);
					colliderB->OnCollision(Collider::CollisionPhase::Stay, colliderA);

				}else if (!isCollision && state.preCollision) {
					//離れた瞬間
					colliderA->OnCollision(Collider::CollisionPhase::Exit, colliderB);
					colliderB->OnCollision(Collider::CollisionPhase::Exit, colliderA);

				}
				

				state.preCollision = isCollision;
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
