#pragma once
///---------------------------------------------------------------------------------------------
// 
// プレイヤーの攻撃クラス
// 
///---------------------------------------------------------------------------------------------

#include "BaseAttack.h"
#include "WorldTransform.h"
#include "CollisionShapes.h"
#include "SphereCollider.h"
#include "GPUParticle.h"

#include <optional>
#include <functional>
#include <map>


//プレイヤー攻撃クラス
class MagicBall : public BaseAttack {
private:

	enum class Phase {
		//通常
		kRoot,
		//発車
		kShot,
	};

	Phase phase_ = Phase::kRoot;
	std::optional<Phase> phaseRequest_ = Phase::kRoot;
	//フェーズ初期化テーブル
	std::map<Phase, std::function<void()>> phaseInitTable_{
		{Phase::kRoot,[this]() { RootInit(); }},
		{Phase::kShot,[this]() { ShotInit(); }},
	};
	//フェーズ更新テーブル
	std::map<Phase, std::function<void()>> phaseUpdateTable_{
		{Phase::kRoot,[this]() { RootUpdate(); }},
		{Phase::kShot,[this]() { ShotUpdate(); }},
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
	/// 発車初期化
	/// </summary>
	void ShotInit();
	/// <summary>
	/// 発射更新
	/// </summary>
	void ShotUpdate();


public:

	~MagicBall();
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
	/// <param name="camera">カメラ</param>
	void DrawParticle(const DaiEngine::Camera& camera) override;
	/// <summary>
	/// 衝突時
	/// </summary>
	void OnCollision(DaiEngine::Collider* other) override;

	void AttackStart(const Vector3& startPos, const Vector3& direction);
	/// <summary>
	/// /存在しているか
	/// </summary>
	/// <returns>存在していればtrue、それ以外はfalse</returns>
	bool IsLife() const { return isLife_; }
	/// <summary>
	/// コライダー取得
	/// </summary>
	/// <returns>コライダー</returns>
	DaiEngine::SphereCollider* GetCollider() const { return collider_.get(); }

private:

	void Dead();

private:
	//オブジェクト
	DaiEngine::WorldTransform worldTransform_;
	//衝突判定用
	std::unique_ptr<DaiEngine::SphereCollider> collider_;
	//スピード
	float speed_ = 0.5f;
	//速度
	Vector3 velocity_{};
	//生存フラグ
	bool isLife_ = true;
	//エフェクト
	std::map<std::string, std::unique_ptr<DaiEngine::GPUParticle>> trailEff_;
	std::map<std::string, std::unique_ptr<DaiEngine::GPUParticle>> endEff_;

	//攻撃開始地点
	Vector3 startPos_{};

	//射程
	float firingRange_ = 20.0f;

	int32_t damage_ = 1;

};

