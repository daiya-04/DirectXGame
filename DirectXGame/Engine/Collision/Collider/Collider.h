#pragma once
#include "CollisionShapes.h"
#include "WorldTransform.h"
#include "Vec3.h"
#include <variant>
#include <functional>
#include <string>
#include <map>

namespace DaiEngine {
	class Collider {
	public:

		enum class Type {
			Sphere,
			OBB,
		};

	protected:

		std::variant<Shapes::Sphere, Shapes::OBB> shape_;
		Type shapeType_;
		std::function<void(Collider*)> callbackFunc_;

		WorldTransform worldTransform_;

		std::string tag_;

		bool isHit_ = false;



	public:

		void SetCallbackFunc(std::function<void(Collider*)> callbackFunc) {
			callbackFunc_ = callbackFunc;
		}

		void OnCollision(Collider* collider) {
			callbackFunc_(collider);
		}

		const std::string GetTag() const { return tag_; }

		const Type GetType() const { return shapeType_; }
		const std::variant<Shapes::Sphere, Shapes::OBB> GetShape() const { return shape_; }

		void SetPosition(const Vector3& position) { worldTransform_.translation_ = position; }
		Vector3 GetWorldPos() const;


		void ColliderOn() { isHit_ = true; }
		void ColliderOff() { isHit_ = false; }
		bool IsHit() const { return isHit_; }


	};
}
