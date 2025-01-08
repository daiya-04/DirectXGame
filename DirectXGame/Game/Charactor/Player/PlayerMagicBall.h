#pragma once
///---------------------------------------------------------------------------------------------
// 
// プレイヤーの攻撃クラス
// 
///---------------------------------------------------------------------------------------------

#include "WorldTransform.h"
#include "Camera.h"
#include "Object3d.h"
#include "Vec3.h"
#include "CollisionShapes.h"
#include "SphereCollider.h"
#include "GPUParticle.h"
#include <memory>
#include <optional>
#include <functional>
#include <map>


//プレイヤー攻撃クラス
class PlayerMagicBall {
public:

	enum class Phase {
		//通常
		kRoot,
		//発車
		kShot,
	};

private:

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
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="startPos">開始位置座標</param>
	/// <param name="direction">発射方向</param>
	void Init(std::shared_ptr<Model> model);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// モデル描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(const Camera& camera);
	/// <summary>
	/// パーティクル描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void DrawParticle(const Camera& camera);
	/// <summary>
	/// 衝突時
	/// </summary>
	void OnCollision(Collider* other);

	void StartAttack(const Vector3& startPos, const Vector3& direction);
	/// <summary>
	/// /存在しているか
	/// </summary>
	/// <returns>存在していればtrue、それ以外はfalse</returns>
	bool IsLife() const { return isLife_; }
	/// <summary>
	/// コライダー取得
	/// </summary>
	/// <returns>コライダー</returns>
	SphereCollider* GetCollider() const { return collider_; }
	/// <summary>
	/// ワールド座標取得
	/// </summary>
	/// <returns>ワールド座標</returns>
	Vector3 GetWorldPos() const;

private:
	//オブジェクト
	std::unique_ptr<Object3d> obj_;
	//衝突判定用
	SphereCollider* collider_ = nullptr;
	//スピード
	float speed_ = 0.5f;
	//速度
	Vector3 velocity_{};
	//生存フラグ
	bool isLife_ = true;
	//エフェクト
	std::map<std::string, std::unique_ptr<GPUParticle>> trailEff_;
	std::map<std::string, std::unique_ptr<GPUParticle>> endEff_;

	//攻撃開始地点
	Vector3 startPos_{};

	//射程
	float firingRange_ = 20.0f;

};

