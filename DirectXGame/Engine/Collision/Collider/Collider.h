#pragma once
#include "CollisionShapes.h"
#include "WorldTransform.h"
#include "Vec3.h"
#include <variant>
#include <functional>
#include <string>
#include <map>
#include <array>

namespace DaiEngine {
	class Collider {
	public:

		enum class Type {
			Sphere,
			OBB,
		};

		enum class CollisionPhase {
			Enter,
			Stay,
			Exit,

			PhaseNum,
		};

	protected:

		std::variant<Shapes::Sphere, Shapes::OBB> shape_;
		Type shapeType_;
		std::array<std::function<void(Collider*)>, static_cast<size_t>(CollisionPhase::PhaseNum)> callbackFuncs_;

		WorldTransform worldTransform_;

		std::string tag_;

		bool isHit_ = false;

		struct Item {
			std::variant<int32_t, float, Vector3> value;
		};

		std::map<std::string, Item> items_;

	public:

		void SetEnterCallback(std::function<void(Collider*)> callbackFunc) {
			callbackFuncs_[static_cast<size_t>(CollisionPhase::Enter)] = callbackFunc;
		}

		void SetStayCallback(std::function<void(Collider*)> callbackFunc) {
			callbackFuncs_[static_cast<size_t>(CollisionPhase::Stay)] = callbackFunc;
		}

		void SetExitCallback(std::function<void(Collider*)> callbackFunc) {
			callbackFuncs_[static_cast<size_t>(CollisionPhase::Exit)] = callbackFunc;
		}

		void OnCollision(CollisionPhase phase, Collider* collider) {
			auto& callbackFunc = callbackFuncs_[static_cast<size_t>(phase)];
			if (callbackFunc) { callbackFunc(collider); }
		}

		void SetValue(const std::string& name, int32_t value) { items_[name].value = value; }
		void SetValue(const std::string& name, float value) { items_[name].value = value; }
		void SetValue(const std::string& name, const Vector3& value) { items_[name].value = value; }

		int32_t GetIntValue(const std::string& name);
		float GetFloatValue(const std::string& name);
		Vector3 GetVec3Value(const std::string& name);

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
