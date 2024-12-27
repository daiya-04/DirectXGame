#pragma once
///---------------------------------------------------------------------------------------------
// 
// 地面から陽が噴き出る攻撃
// ボスの攻撃の一種
// 
///---------------------------------------------------------------------------------------------

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

//地面から炎攻撃のクラス
class GroundFlare {
public:
	//フェーズ
	enum class Phase {
		//通常
		kRoot,
		//警告
		kWarning,
		//発射
		kFire,
	};
	//現在のフェーズ
	Phase phase_ = Phase::kRoot;
	//次にフェーズのリクエスト
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
	//警告地点に必要なパラメータ
	struct WorkWarning {
		//イージング用
		float param_ = 0.0f;
		//円が広がるスピード
		float speed_ = 0.01f;
		//開始スケール
		Vector3 startScale_ = { 0.0f, 1.0f, 0.0f };
		//終了スケール
		Vector3 endScale_ = { 1.0f, 1.0f, 1.0f };
	};
	//炎の発射に必要なパラメータ
	struct WorkFire {
		//発射時間
		int32_t fireCount_ = 60 * 1;
		//発射中の現在の時間
		int32_t param_ = 0;
	};
	//警告用パラメータ
	WorkWarning workWarning_;
	//発射用パラメータ
	WorkFire workFire_;

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
	/// 警告初期化
	/// </summary>
	void WarningInit();
	/// <summary>
	/// 警告更新
	/// </summary>
	void WarningUpdate();
	/// <summary>
	/// 発射初期化
	/// </summary>
	void FireInit();
	/// <summary>
	/// 発射更新
	/// </summary>
	void FireUpdate();


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
	/// ターゲットのセット
	/// </summary>
	/// <param name="target">ターゲットのワールドトランスフォーム</param>
	void SetTerget(const WorldTransform* target) { target_ = target; }
	/// <summary>
	/// コライダー取得
	/// </summary>
	/// <returns>コライダー</returns>
	Shapes::OBB GetCollider() { return collider_; }
	/// <summary>
	/// フラグ取得
	/// </summary>
	/// <returns>攻撃中ならtrue、それ以外false</returns>
	bool IsAttack() const { return isAttack_; }
	/// <summary>
	/// 当たり判定が有効か
	/// </summary>
	/// <returns>有効ならture、それ以外ならfalse</returns>
	bool IsHit() const { return isHit_; }
	/// <summary>
	/// 攻撃が終了したか
	/// </summary>
	/// <returns>攻撃が終了した瞬間true、それ以外はfalse</returns>
	bool AttackFinish() const { return (!isAttack_ && preIsAttack_); }
	/// <summary>
	/// 攻撃が始まったか
	/// </summary>
	/// <returns>攻撃が開始した瞬間true、それ以外はfalse</returns>
	bool FireStartFlag() const { return(isHit_ && !preIsHit_); }

	Vector3 GetCenterPos() { return centerPos_; }

private:
	//攻撃先(ターゲット)
	const WorldTransform* target_;
	//噴射する数
	static const uint32_t flareNum_ = 5;
	//エフェクト
	std::array<std::map<std::string, std::unique_ptr<GPUParticle>>, flareNum_> fires_;

	//警告の円
	std::array<std::unique_ptr<Object3d>, flareNum_> warningZones_;
	//警告円のオフセット
	std::array<Vector3, flareNum_> offset_;
	//オフセットの距離
	const float offsetLength_ = 1.5f;
	//コライダー(形状)
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

