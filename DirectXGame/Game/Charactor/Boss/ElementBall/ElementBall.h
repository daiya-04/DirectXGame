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
	//フェーズ
	enum class Phase {
		kRoot,
		kSet,
		kCharge,
		kShot,
	};

	Phase phase_ = Phase::kRoot;
	std::optional<Phase> phaseRequest_ = Phase::kRoot;
	//フェーズ初期化テーブル
	std::map<Phase, std::function<void()>> phaseInitTable_{
		{Phase::kRoot,[this]() {RootInit(); }},
		{Phase::kSet,[this]() {SetInit(); }},
		{Phase::kCharge,[this]() {ChargeInit(); }},
		{Phase::kShot,[this]() {ShotInit(); }},
	};
	//フェーズ更新テーブル
	std::map<Phase, std::function<void()>> phaseUpdateTable_{
		{Phase::kRoot,[this]() {RootUpdate(); }},
		{Phase::kSet,[this]() {SetUpdate(); }},
		{Phase::kCharge,[this]() {ChargeUpdate(); }},
		{Phase::kShot,[this]() {ShotUpdate(); }},
	};

private:
	//通常
	void RootInit();
	void RootUpdate();
	//攻撃セット
	void SetInit();
	void SetUpdate();
	//溜め
	void ChargeInit();
	void ChargeUpdate();
	//発射
	void ShotInit();
	void ShotUpdate();

private:
	//セット時に必要なパラメータ
	struct WorkSet {
		Vector3 start{};
		Vector3 end{};
		float param = 0.0f;
	};
	//溜めに必要なパラメータ
	struct WorkCharge {
		uint32_t coolTime = 60;
		uint32_t param = 0;
	};
	//発射に必要なパラメータ
	struct WorkShot {
		bool isTrack = true;
		const float trackingDist = 25.0f;
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
	//エフェクト
	std::unique_ptr<GPUParticle> core_;
	std::unique_ptr<GPUParticle> smoke_;

public:
	//初期化
	void Init(std::shared_ptr<Model> model);
	//更新
	void Update();
	void ColliderUpdate() {
		collider_.center = obj_->GetWorldPos();
		collider_.radius = size_.x;
	}
	//描画
	void Draw(const Camera& camera);
	void DrawParticle(const Camera& camera);
	//衝突時
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

