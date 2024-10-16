#pragma once
#include "WorldTransform.h"
#include "Camera.h"
#include "Object3d.h"
#include "Vec3.h"
#include "CollisionShapes.h"
#include "GPUParticle.h"

#include <memory>
#include <optional>
#include <functional>
#include <map>


class ElementBall {
public:

	enum class Phase {
		kRoot,
		kSet,
		kCharge,
		kShot,
	};

	Phase phase_ = Phase::kRoot;
	std::optional<Phase> phaseRequest_ = Phase::kRoot;

	std::map<Phase, std::function<void()>> phaseInitTable_{
		{Phase::kRoot,[this]() {RootInit(); }},
		{Phase::kSet,[this]() {SetInit(); }},
		{Phase::kCharge,[this]() {ChargeInit(); }},
		{Phase::kShot,[this]() {ShotInit(); }},
	};

	std::map<Phase, std::function<void()>> phaseUpdateTable_{
		{Phase::kRoot,[this]() {RootUpdate(); }},
		{Phase::kSet,[this]() {SetUpdate(); }},
		{Phase::kCharge,[this]() {ChargeUpdate(); }},
		{Phase::kShot,[this]() {ShotUpdate(); }},
	};

private:

	void RootInit();
	void RootUpdate();
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

	WorkSet workSet_;
	WorkCharge workCharge_;
	WorkShot workShot_;

private:

	const WorldTransform* target_;

	std::unique_ptr<Object3d> obj_;
	Animation animation_;
	Vector3 size_ = { 1.3f,1.3f,1.3f };

	Shapes::Sphere collider_{};

	bool isLife_ = false;
	bool preIsLife_ = false;

	std::unique_ptr<GPUParticle> particle_;

public:

	void Init(std::shared_ptr<Model> model);

	void Update();
	void ColliderUpdate() {
		collider_.center = obj_->GetWorldPos();
		collider_.radius = size_.x;
	}

	void Draw(const Camera& camera);

	void DrawParticle(const Camera& camera);

	void OnCollision();

	void SetTarget(const WorldTransform* target) { target_ = target; }

	void SetShotCount(uint32_t seconds) { workCharge_.coolTime = 60 * seconds; }

	void AttackStart();
	void SetAttackData(const Vector3& startPos, uint32_t interval);

	bool IsLife() const { return isLife_; }

	bool DeadFlag() const { return (!isLife_ && preIsLife_); }

	Shapes::Sphere GetCollider() const { return collider_; }
	Phase GetPhase() const { return phase_; }
	const Vector3 GetWorldPos() const { return obj_->GetWorldPos(); }

};

