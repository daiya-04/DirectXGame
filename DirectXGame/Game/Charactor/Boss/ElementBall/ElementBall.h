#pragma once
///---------------------------------------------------------------------------------------------
// 
// 火の玉攻撃
// ボスの攻撃の一種
// 
///---------------------------------------------------------------------------------------------

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

//火の玉攻撃クラス
class ElementBall {
public:
	//フェーズ
	enum class Phase {
		//通常
		kRoot,
		//攻撃セット
		kSet,
		//溜め
		kCharge,
		//発射
		kShot,
	};

	//現在のフェーズ
	Phase phase_ = Phase::kRoot;
	//次のフェーズのリクエスト
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
	/// <summary>
	/// 通常初期化
	/// </summary>
	void RootInit();
	/// <summary>
	/// 通常更新
	/// </summary>
	void RootUpdate();
	/// <summary>
	/// 攻撃セット初期化
	/// </summary>
	void SetInit();
	/// <summary>
	/// 攻撃セット更新
	/// </summary>
	void SetUpdate();
	/// <summary>
	/// 溜め初期化
	/// </summary>
	void ChargeInit();
	/// <summary>
	/// 溜め更新
	/// </summary>
	void ChargeUpdate();
	/// <summary>
	/// 発射初期化
	/// </summary>
	void ShotInit();
	/// <summary>
	/// 発射更新
	/// </summary>
	void ShotUpdate();

private:
	
	//溜めに必要なパラメータ
	struct WorkCharge {
		//溜め時間
		uint32_t coolTime_ = 60;
		//溜め中の現在の時間
		uint32_t param_ = 0;
	};
	//発射に必要なパラメータ
	struct WorkShot {
		//追尾中か
		bool isTrack_ = true;
		//津美が終わる距離
		const float trackingDist_ = 25.0f;
	};

	//溜め用パラメータ
	WorkCharge workCharge_;
	//発射用パラメータ
	WorkShot workShot_;

private:

	//攻撃先(ターゲット)
	const WorldTransform* target_;
	//オブジェクト
	std::unique_ptr<Object3d> obj_;
	//アニメーション
	Animation animation_;

	//速度
	Vector3 velocity_ = {};

	//コライダー(形状)
	Shapes::Sphere collider_{};
	//生存フラグ
	bool isLife_ = false;
	bool preIsLife_ = false;

	//エフェクト
	std::map <std::string, std::unique_ptr<GPUParticle>> effect_;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	void Init(std::shared_ptr<Model> model);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// コライダー更新
	/// </summary>
	void ColliderUpdate() {
		collider_.center = obj_->GetWorldPos();
	}
	/// <summary>
	/// 描画
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
	void OnCollision();
	/// <summary>
	/// ターゲットのセット
	/// </summary>
	/// <param name="target">ターゲットのワールドトランスフォーム</param>
	void SetTarget(const WorldTransform* target) { target_ = target; }
	/// <summary>
	/// 攻撃開始
	/// </summary>
	void AttackStart();
	/// <summary>
	/// 攻撃に必要なパラメータの設定
	/// </summary>
	/// <param name="startPos">開始座標</param>
	/// <param name="interval"></param>
	void SetAttackData(const Vector3& startPos, uint32_t interval);
	/// <summary>
	/// 生存しているか
	/// </summary>
	/// <returns>存在していればtrue、それ以外はfalse</returns>
	bool IsLife() const { return isLife_; }
	/// <summary>
	/// 死んだ瞬間
	/// </summary>
	/// <returns>死んだ瞬間ならtrue、それ以外はfalse</returns>
	bool DeadFlag() const { return (!isLife_ && preIsLife_); }
	/// <summary>
	/// コライダー取得
	/// </summary>
	/// <returns>コライダー</returns>
	Shapes::Sphere GetCollider() const { return collider_; }
	/// <summary>
	/// 現在のフェーズ取得
	/// </summary>
	/// <returns>フェーズ</returns>
	Phase GetPhase() const { return phase_; }
	/// <summary>
	/// ワールド座標取得
	/// </summary>
	/// <returns>ワールド座標</returns>
	const Vector3 GetWorldPos() const { return obj_->GetWorldPos(); }

};

