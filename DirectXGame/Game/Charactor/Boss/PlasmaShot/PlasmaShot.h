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


class PlasmaShot{
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
	//生成
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
	void ColliderUpdate();
	//描画
	void Draw(const Camera& camera);
	void DrawParticle(const Camera& camera);
	//衝突時
	void OnCollision();

	void AttackStart();

	void SetTarget(const WorldTransform* target) { target_ = target; }
	void SetAttackData(const Vector3& pos, float interval);

	
	Shapes::Sphere GetCollider() const { return collider_; }
	bool IsLife() const { return isLife_; }

private:
	
	const WorldTransform* target_;
	Vector3 targetDict_ = { 0.0f,0.0f,1.0f };

	std::unique_ptr<Object3d> obj_;
	//エフェクト
	std::unique_ptr<GPUParticle> particle_;
	std::unique_ptr<GPUParticle> hitEff_;
	std::unique_ptr<GPUParticle> hitSpark_;
	std::unique_ptr<GPUParticle> createEff_;

	Shapes::Sphere collider_;

	Vector3 velocity_;
	float speed_ = 0.7f;

	bool isLife_ = false;

private:
	//生成に必要なパラメータ
	struct CreateData {
		float param_ = 0.0f;
	};
	//待ちに必要なパラメータ
	struct WaitData {
		int32_t count_ = 0;
		int32_t waitTime_ = 60 * 1;
	};

	CreateData createData_;
	WaitData waitData_;
};

