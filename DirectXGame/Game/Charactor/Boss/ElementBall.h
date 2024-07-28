#pragma once
#include "WorldTransform.h"
#include "Camera.h"
#include "Object3d.h"
#include "Vec3.h"
#include "CollisionShapes.h"
#include <memory>
#include <optional>


class ElementBall {
public:

	enum class Phase {
		kSet,
		kCharge,
		kShot,
	};

	Phase phase_ = Phase::kSet;
	std::optional<Phase> phaseRequest_ = std::nullopt;

private:

	void SetInit();
	void SetUpdate();
	void ChargeInit();
	void ChargeUpdate();
	void ShotInit();
	void ShotUpdate();

private:

	struct WorkSet {
		Vector3 start{};
		Vector3 end{};
		float param = 0.0f;
	};

	struct WorkCharge {
		uint32_t coolTime = 60;
		uint32_t param = 0;
	};

	struct WorkShot {
		bool isTrack = true;
		const float trackingDist = 25;
		Vector3 move{};
	};

private:

	static const WorldTransform* target_;

	std::unique_ptr<Object3d> obj_;
	Animation animation_;
	Vector3 size_ = { 1.3f,1.3f,1.3f };

	Shapes::Sphere collider_{};

	bool isLife_ = false;

	WorkSet workSet_;
	WorkCharge workCharge_;
	WorkShot workShot_;

public:

	void Init(std::shared_ptr<Model> model,const Vector3& startPos);

	void Update();
	void ColliderUpdate() {
		collider_.center = obj_->GetWorldPos();
		collider_.radius = size_.x;
	}

	void Draw(const Camera& camera);

	void OnCollision();

	static void SetTarget(const WorldTransform* target) { target_ = target; }

	void SetShotCount(uint32_t seconds) { workCharge_.coolTime = 60 * seconds; }

	bool IsLife() const { return isLife_; }

	Shapes::Sphere GetCollider() const { return collider_; }
	Phase GetPhase() const { return phase_; }
	const Vector3 GetWorldPos() const { return obj_->GetWorldPos(); }

};

