#pragma once
#include "BaseAttack.h"
#include "WorldTransform.h"
#include "CollisionShapes.h"
#include "SphereCollider.h"
#include "GPUParticle.h"

#include <optional>
#include <functional>
#include <map>


class MagicCannon : public BaseAttack {
private:

	enum class Phase {
		kRoot,
		kShot,
	};

	Phase phase_ = Phase::kRoot;
	std::optional<Phase> phaseRequest_ = Phase::kRoot;
	//フェーズ初期化テーブル
	std::map<Phase, std::function<void()>> phaseInitTable_{
		{Phase::kRoot, [this]() {RootInit(); }},
		{Phase::kShot, [this]() {ShotInit(); }},
	};
	//フェーズ更新テーブル
	std::map<Phase, std::function<void()>> phaseUpdateTable_{
		{Phase::kRoot, [this]() {RootUpdate(); }},
		{Phase::kShot, [this]() {ShotUpdate(); }},
	};



private:
	/// <summary>
	/// 通常初期化
	/// </summary>
	void RootInit();
	/// <summary>
	/// 通常更新
	/// </summary>
	void RootUpdate();
	/// <summary>
	/// 発射初期化
	/// </summary>
	void ShotInit();
	/// <summary>
	/// 発射更新
	/// </summary>
	void ShotUpdate();

public:

	~MagicCannon();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override;
	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;
	/// <summary>
	/// パーティクル描画
	/// </summary>
	/// <param name="camera"></param>
	void DrawParticle(const DaiEngine::Camera& camera) override;
	/// <summary>
	/// 衝突時
	/// </summary>
	/// <param name="other"></param>
	void OnCollision(DaiEngine::Collider* other) override;

	void AttackStart(const Vector3& startPos, const Vector3& direction);

private:

	void Dead();

private:

	DaiEngine::WorldTransform worldTransform_;

	std::unique_ptr<DaiEngine::SphereCollider> collider_;
	//速さ
	float speed_ = 0.7f;
	//速度
	Vector3 velocity_{};

	//エフェクト
	std::map<std::string, std::unique_ptr<DaiEngine::GPUParticle>> effect_;
	std::map<std::string, std::unique_ptr<DaiEngine::GPUParticle>> endEff_;


	//攻撃開始座標
	Vector3 startPos_{};
	//射程
	float firingRange_ = 20.0f;

};

