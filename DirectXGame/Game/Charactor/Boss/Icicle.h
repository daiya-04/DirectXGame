#pragma once
#include "ModelManager.h"
#include "Vec3.h"
#include "Vec2.h"
#include "Matrix44.h"
#include "WorldTransform.h"
#include "Camera.h"
#include "Object3d.h"
#include "CollisionShapes.h"
#include "GPUParticle.h"

#include <memory>
#include <string>
#include <optional>
#include <functional>
#include <map>


class Icicle {
public:

	enum class Phase {
		kRoot,
		kCreate,
		kWait,
		kShot,
	};

private:

	Phase phase_ = Phase::kRoot;
	std::optional<Phase> phaseRequest_ = Phase::kRoot;

	std::map<Phase, std::function<void()>> phaseInitTable_{
		{Phase::kRoot,[this]() {RootInit(); }},
		{Phase::kCreate,[this]() {CreateInit(); }},
		{Phase::kWait,[this]() {WaitInit(); }},
		{Phase::kShot,[this]() {ShotInit(); }},
	};
	std::map<Phase, std::function<void()>> phaseUpdateTable_{
		{Phase::kRoot,[this]() {RootUpdate(); }},
		{Phase::kCreate,[this]() {CreateUpdate(); }},
		{Phase::kWait,[this]() {WaitUpdate(); }},
		{Phase::kShot,[this]() {ShotUpdate(); }},
	};

private:

	void RootInit();
	void RootUpdate();

	void CreateInit();
	void CreateUpdate();

	void WaitInit();
	void WaitUpdate();

	void ShotInit();
	void ShotUpdate();


public:

	void Init(const std::shared_ptr<Model>& model);

	void Update();
	void ColliderUpdate() {
		collider_.center = GetWorldPos();
	}

	void Draw(const Camera& camera);
	void DrawParticle(const Camera& camera);

	void OnCollision();
	void AttackStart();

	void SetTarget(const WorldTransform* target) { target_ = target; }
	void SetAttackData(const Vector3& pos, const Vector3& direction);

	Vector3 GetWorldPos() const { return obj_->GetWorldPos(); }
	Shapes::Sphere GetCollider() { return collider_; }
	bool IsLife() const { return isLife_; }

private:

	const WorldTransform* target_;

	std::unique_ptr<Object3d> obj_;
	Matrix4x4 rotateMat_ = MakeIdentity44();

	std::unique_ptr<GPUParticle> particle_;

	Shapes::Sphere collider_;

	Vector3 velocity_;
	float speed_ = 0.6f;

	Vector3 direction_;
	Vector3 targetDict_;

	bool isLife_ = false;

private:

	struct CreateData {
		float param_ = 0.0f;
	};

	struct WaitData {
		int32_t count_ = 0;
		int32_t waitTime_ = 60 * 1;
	};

	struct ShotData {
		float param_ = 0.0f;
		float trackingDist_ = 10.0f;
	};

	CreateData createData_;
	WaitData waitData_;
	ShotData shotData_;


};

