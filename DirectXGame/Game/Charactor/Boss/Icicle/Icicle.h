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
	//フェーズ
	enum class Phase {
		kRoot,
		kCreate,
		kWait,
		kShot,
	};

private:

	Phase phase_ = Phase::kRoot;
	std::optional<Phase> phaseRequest_ = Phase::kRoot;
	//フェーズ初期化テーブル
	std::map<Phase, std::function<void()>> phaseInitTable_{
		{Phase::kRoot,[this]() {RootInit(); }},
		{Phase::kCreate,[this]() {CreateInit(); }},
		{Phase::kWait,[this]() {WaitInit(); }},
		{Phase::kShot,[this]() {ShotInit(); }},
	};
	//フェーズ更新テーブル
	std::map<Phase, std::function<void()>> phaseUpdateTable_{
		{Phase::kRoot,[this]() {RootUpdate(); }},
		{Phase::kCreate,[this]() {CreateUpdate(); }},
		{Phase::kWait,[this]() {WaitUpdate(); }},
		{Phase::kShot,[this]() {ShotUpdate(); }},
	};

private:
	//通常
	void RootInit();
	void RootUpdate();
	//攻撃生成
	void CreateInit();
	void CreateUpdate();
	//待ち
	void WaitInit();
	void WaitUpdate();
	//発射
	void ShotInit();
	void ShotUpdate();


public:
	//初期化
	void Init(const std::shared_ptr<Model>& model);
	//更新
	void Update();
	void ColliderUpdate() {
		collider_.center = GetWorldPos();
	}
	//描画
	void Draw(const Camera& camera);
	void DrawParticle(const Camera& camera);
	//衝突時
	void OnCollision();

	void AttackStart();

	void SetTarget(const WorldTransform* target) { target_ = target; }
	void SetAttackData(const Vector3& pos, const Vector3& direction, float interval);

	Vector3 GetWorldPos() const { return obj_->GetWorldPos(); }
	Shapes::Sphere GetCollider() { return collider_; }
	bool IsLife() const { return isLife_; }
	bool DeadFlag() const { return (!isLife_ && preIsLife_); }

private:

	const WorldTransform* target_;

	std::unique_ptr<Object3d> obj_;
	Matrix4x4 rotateMat_ = MakeIdentity44();

	//エフェクト
	//std::unique_ptr<GPUParticle> particle_;
	//std::unique_ptr<GPUParticle> iceCreate_;
	//std::unique_ptr<GPUParticle> iceSpark_;
	//std::unique_ptr<GPUParticle> coolAir_;
	std::map<std::string, std::unique_ptr<GPUParticle>> createEffect_;
	std::map<std::string, std::unique_ptr<GPUParticle>> hitEffect_;
	std::map<std::string, std::unique_ptr<GPUParticle>> trailEff_;

	Shapes::Sphere collider_;

	Vector3 velocity_;
	float speed_ = 0.6f;

	//向き
	Vector3 direction_;
	//ターゲットのいる方向
	Vector3 targetDict_;

	bool isLife_ = false;
	bool preIsLife_ = false;

private:
	//生成に必要なパラメータ
	struct CreateData {
		float param_ = 0.0f;
	};
	//待ちに必要なパラメータ
	struct WaitData {
		int32_t count_ = 0;
		int32_t waitTime_ = static_cast<int32_t>(60.0f * 1.5f);
	};
	//発射に必要なパラメータ
	struct ShotData {
		float param_ = 0.0f;
		float trackingDist_ = 10.0f;
	};

	CreateData createData_;
	WaitData waitData_;
	ShotData shotData_;


};

