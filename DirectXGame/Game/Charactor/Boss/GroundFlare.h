#pragma once
#include "WorldTransform.h"
#include "Camera.h"
#include "Object3d.h"
#include "GPUParticle.h"
#include "Vec3.h"
#include "CollisionShapes.h"

#include <memory>
#include <array>
#include <optional>
#include <map>
#include <functional>


class GroundFlare {
public:

	enum class Phase {
		kRoot,
		kWarning,
		kFire,
	};

	Phase phase_ = Phase::kRoot;
	std::optional<Phase> phaseRequest_ = Phase::kRoot;

	std::map<Phase, std::function<void()>> phaseInitTable_{
		{Phase::kRoot,[this]() {RootInit(); }},
		{Phase::kWarning,[this]() {WarningInit(); }},
		{Phase::kFire,[this]() {FireInit(); }},
	};

	std::map<Phase, std::function<void()>> phaseUpdateTable_{
		{Phase::kRoot,[this]() {RootUpdate(); }},
		{Phase::kWarning,[this]() {WarningUpdate(); }},
		{Phase::kFire,[this]() {FireUpdate(); }},
	};

private:

	struct WorkWarning {
		float param_ = 0.0f;
	};

	struct WorkFire {
		int32_t fireCount_ = 60 * 1;
		int32_t param_ = 0;
	};

	WorkWarning workWarning_;
	WorkFire workFire_;

private:
	void RootInit();
	void RootUpdate();
	void WarningInit();
	void WarningUpdate();
	void FireInit();
	void FireUpdate();


public:

	void Init(std::shared_ptr<Model> model);

	void Update();
	void ColliderUpdate();

	void Draw(const Camera& camera);
	void DrawParticle(const Camera& camera);

	void OnCollision();

	void AttackStart();

	void SetTerget(const WorldTransform* target) { target_ = target; }

	Shapes::OBB GetCollider() { return collider_; }

	bool IsAttack() const { return isAttack_; }
	bool IsHit() const { return isHit_; }

	bool AttackFinish() const { return (!isAttack_ && preIsAttack_); }
	bool FireStartFlag() const { return(isHit_ && !preIsHit_); }

private:

	const WorldTransform* target_;

	static const uint32_t flareNum_ = 5;

	std::array<std::unique_ptr<GPUParticle>, flareNum_> particles_;

	//予測
	std::array<std::unique_ptr<Object3d>, flareNum_> warningZones_;
	std::array<Vector3, flareNum_> offset_;

	Shapes::OBB collider_;

	Vector3 centerPos_{};

	//攻撃中か
	bool isAttack_ = false;
	bool preIsAttack_ = false;
	//衝突判定の有無
	bool isHit_ = false;
	bool preIsHit_ = false;

};

