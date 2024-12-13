#pragma once
///---------------------------------------------------------------------------------------------
// 
// つらら攻撃
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

//つらら攻撃クラス
class Icicle {
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
	void ColliderUpdate() {
		collider_.center = GetWorldPos();
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
	/// <param name="direction">初期方向</param>
	/// <param name="interval">待ち時間</param>
	void SetAttackData(const Vector3& pos, const Vector3& direction, float interval);
	/// <summary>
	/// ワールド座標取得
	/// </summary>
	/// <returns>ワールド座標</returns>
	Vector3 GetWorldPos() const { return obj_->GetWorldPos(); }
	/// <summary>
	/// コライダー取得
	/// </summary>
	/// <returns>コライダー</returns>
	Shapes::Sphere GetCollider() { return collider_; }
	/// <summary>
	/// 生存しているか
	/// </summary>
	/// <returns>存在していればtrue、それ以外はfalse</returns>
	bool IsLife() const { return isLife_; }
	/// <summary>
	/// 消えた瞬間
	/// </summary>
	/// <returns>消えた瞬間true、それ以外はfalse</returns>
	bool DeadFlag() const { return (!isLife_ && preIsLife_); }

private:
	//攻撃先(ターゲット)
	const WorldTransform* target_;
	//オブジェクト
	std::unique_ptr<Object3d> obj_;
	//回転行列
	Matrix4x4 rotateMat_ = MakeIdentity44();

	///エフェクト
	//生成時
	std::map<std::string, std::unique_ptr<GPUParticle>> createEffect_;
	//ヒット時
	std::map<std::string, std::unique_ptr<GPUParticle>> hitEffect_;
	//軌跡
	std::map<std::string, std::unique_ptr<GPUParticle>> trailEff_;
	///

	//コライダー(形状)
	Shapes::Sphere collider_;

	//速度
	Vector3 velocity_;
	//スピード
	float speed_ = 0.6f;

	//向き
	Vector3 direction_;
	//ターゲットのいる方向
	Vector3 targetDict_;

	//生存フラグ
	bool isLife_ = false;
	bool preIsLife_ = false;

private:
	//生成に必要なパラメータ
	struct CreateData {
		//生成中の現在の時間
		float param_ = 0.0f;
		//生成スピード
		float speed_ = 0.01f;
		//開始スケール
		Vector3 startScale_ = {};
		//終了スケール
		Vector3 endScale_ = { 0.5f,0.5f,0.5f };
	};
	//待ちに必要なパラメータ
	struct WaitData {
		//待ち中の現在の時間
		int32_t count_ = 0;
		//待ち時間
		int32_t waitTime_ = static_cast<int32_t>(60.0f * 1.5f);
	};
	//発射に必要なパラメータ
	struct ShotData {
		//補間用媒介変数表示
		float param_ = 0.0f;
		//追尾が終わる距離
		float trackingDist_ = 10.0f;
		//回転スピード
		float rotateSpeed_ = 0.005f;
	};

	//生成用パラメータ
	CreateData createData_;
	//待ち用パラメータ
	WaitData waitData_;
	//発射用パラメータ
	ShotData shotData_;


};

