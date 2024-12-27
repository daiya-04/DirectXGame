#pragma once
///---------------------------------------------------------------------------------------------
// 
// 電気玉
// ボスの攻撃の一種
// 
///---------------------------------------------------------------------------------------------

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

//電気玉クラス
class PlasmaShot{
public:
	//フェーズ
	enum class Phase {
		//通常
		kRoot,
		//生成
		kCreate,
		//待ち
		kWait,
		//発射
		kShot,
	};

private:
	//現在のフェーズ
	Phase phase_ = Phase::kRoot;
	//次のフェーズのリクエスト
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
	/// <summary>
	/// 通常初期化
	/// </summary>
	void RootInit();
	/// <summary>
	/// 通常更新
	/// </summary>
	void RootUpdate();
	/// <summary>
	/// 生成初期化
	/// </summary>
	void CreateInit();
	/// <summary>
	/// 生成更新
	/// </summary>
	void CreateUpdate();
	/// <summary>
	/// 待ち初期化
	/// </summary>
	void WaitInit();
	/// <summary>
	/// 待ち更新
	/// </summary>
	void WaitUpdate();
	/// <summary>
	/// 発射初期化
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
	void Init(const std::shared_ptr<Model>& model);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// コライダー更新
	/// </summary>
	void ColliderUpdate();
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
	/// 攻撃開始
	/// </summary>
	void AttackStart();
	/// <summary>
	/// ターゲットセット
	/// </summary>
	/// <param name="target">ターゲットのワールドトランスフォーム</param>
	void SetTarget(const WorldTransform* target) { target_ = target; }
	/// <summary>
	/// 攻撃に必要なデータの設定
	/// </summary>
	/// <param name="pos">発射座標</param>
	/// <param name="interval">待ち時間</param>
	void SetAttackData(const Vector3& pos, float interval);

	/// <summary>
	/// ワールド座標取得
	/// </summary>
	/// <returns>ワールド座標</returns>
	Vector3 GetWorldPos() const { return obj_->GetWorldPos(); }

	/// <summary>
	/// コライダー取得
	/// </summary>
	/// <returns>コライダー</returns>
	Shapes::Sphere GetCollider() const { return collider_; }
	/// <summary>
	/// 存在しているか
	/// </summary>
	/// <returns>生存していたらtrue、それ以外false</returns>
	bool IsLife() const { return isLife_; }

private:
	//攻撃先(ターゲット)
	const WorldTransform* target_;
	//ターゲットがいる方向
	Vector3 targetDict_ = { 0.0f,0.0f,1.0f };
	//オブジェクト
	std::unique_ptr<Object3d> obj_;
	///エフェクト
	//生成
	std::map<std::string, std::unique_ptr<GPUParticle>> createEff_;
	//ヒット時
	std::map<std::string, std::unique_ptr<GPUParticle>> hitEff_;
	//軌跡
	std::map<std::string, std::unique_ptr<GPUParticle>> trailEff_;
	///

	//コライダー(形状)
	Shapes::Sphere collider_;

	//速度
	Vector3 velocity_;
	//スピード
	float speed_ = 0.7f;
	//生存フラグ
	bool isLife_ = false;

private:
	
	//待ちに必要なパラメータ
	struct WaitData {
		//待ち中の現在の時間
		int32_t count_ = 0;
		//待ち時間
		int32_t waitTime_ = 60 * 1;
	};
	//待ち用のパラメータ
	WaitData waitData_;
};

