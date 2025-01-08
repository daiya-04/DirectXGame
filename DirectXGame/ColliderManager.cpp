#include "ColliderManager.h"
#include "Hit.h"


ColliderManager* ColliderManager::GetInstance() {
	static ColliderManager instance;

	return &instance;
}

SphereCollider* ColliderManager::CreateSphere() {
	return ColliderManager::GetInstance()->InternalCreateSphere();
}

OBBCollider* ColliderManager::CreateOBB() {
	return ColliderManager::GetInstance()->internalCreateOBB();
}

SphereCollider* ColliderManager::InternalCreateSphere() {

	auto newCollider = std::make_unique<SphereCollider>();
	//ポインタ取得
	SphereCollider* returnCollider = newCollider.get();

	//配列に追加
	colliders_.push_back(std::move(newCollider));

	return returnCollider;
}

OBBCollider* ColliderManager::internalCreateOBB() {

	auto newCollider = std::make_unique<OBBCollider>();
	//ポインタ取得
	OBBCollider* returnCollider = newCollider.get();

	//配列に追加
	colliders_.push_back(std::move(newCollider));

	return returnCollider;
}

void ColliderManager::CheckAllCollision() {

	for (size_t i = 0; i < colliders_.size(); i++) {
		Collider* colliderA = colliders_[i].get();

		if (!colliderA->IsHit()) { continue; }

		for (size_t j = 0; j < colliders_.size(); j++) {

			if (i == j) { continue; }
			
			Collider* colliderB = colliders_[j].get();
			if (!colliderB->IsHit()) { continue; }

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
	}else if (typeA == Collider::Type::OBB && typeB == Collider::Type::OBB) {
		return IsCollision(std::get<Shapes::OBB>(shapeA), std::get<Shapes::OBB>(shapeB));
	}else if (typeA == Collider::Type::Sphere && typeB == Collider::Type::OBB) {
		return IsCollision(std::get<Shapes::OBB>(shapeB), std::get<Shapes::Sphere>(shapeA));
	}else if (typeA == Collider::Type::OBB && typeB == Collider::Type::Sphere) {
		return IsCollision(std::get<Shapes::OBB>(shapeA), std::get<Shapes::Sphere>(shapeB));
	}


	return false;
}
