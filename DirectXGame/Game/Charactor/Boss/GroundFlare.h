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
	//フェーズ
	enum class Phase {
		kRoot,
		kWarning,
		kFire,
	};

	Phase phase_ = Phase::kRoot;
	std::optional<Phase> phaseRequest_ = Phase::kRoot;
	//フェーズ初期化テーブル
	std::map<Phase, std::function<void()>> phaseInitTable_{
		{Phase::kRoot,[this]() {RootInit(); }},
		{Phase::kWarning,[this]() {WarningInit(); }},
		{Phase::kFire,[this]() {FireInit(); }},
	};
	//フェーズ更新テーブル
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
	//通常初期化
	void RootInit();
	//通常更新
	void RootUpdate();
	//警告初期化
	void WarningInit();
	//警告更新
	void WarningUpdate();
	//発射初期化
	void FireInit();
	//発射更新
	void FireUpdate();


public:
	//初期化
	void Init(std::shared_ptr<Model> model);
	//更新
	void Update();
	void ColliderUpdate();
	//描画
	void Draw(const Camera& camera);
	void DrawParticle(const Camera& camera);
	//衝突時
	void OnCollision();
	//攻撃開始
	void AttackStart();

	void SetTerget(const WorldTransform* target) { target_ = target; }

	Shapes::OBB GetCollider() { return collider_; }

	bool IsAttack() const { return isAttack_; }
	bool IsHit() const { return isHit_; }

	bool AttackFinish() const { return (!isAttack_ && preIsAttack_); }
	bool FireStartFlag() const { return(isHit_ && !preIsHit_); }

private:

	const WorldTransform* target_;
	//噴射する数
	static const uint32_t flareNum_ = 5;

	//std::array<std::unique_ptr<GPUParticle>, flareNum_> particles_;
	std::array<std::map<std::string, std::unique_ptr<GPUParticle>>, flareNum_> effects_;

	//予測
	std::array<std::unique_ptr<Object3d>, flareNum_> warningZones_;
	std::array<Vector3, flareNum_> offset_;

	Shapes::OBB collider_;
	//中心座標
	Vector3 centerPos_{};

	//攻撃中か
	bool isAttack_ = false;
	bool preIsAttack_ = false;
	//衝突判定の有無
	bool isHit_ = false;
	bool preIsHit_ = false;

};

