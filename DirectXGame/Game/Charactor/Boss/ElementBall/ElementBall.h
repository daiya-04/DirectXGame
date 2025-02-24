#pragma once
///---------------------------------------------------------------------------------------------
// 
// 火の玉攻撃
// ボスの攻撃の一種
// 
///---------------------------------------------------------------------------------------------

#include "BaseAttack.h"
#include "WorldTransform.h"
#include "CollisionShapes.h"
#include "GPUParticle.h"
#include "SphereCollider.h"
#include "BurnScar.h"

#include <optional>
#include <functional>
#include <map>

//火の玉攻撃クラス
class ElementBall : public BaseAttack {
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
	//跡の高さ調整用
	static size_t heightAdjustmentIndex;
	//攻撃先（ターゲット）
	const Vector3* target_ = nullptr;
	//
	std::unique_ptr<DaiEngine::Object3d> obj_;
	//アニメーション
	DaiEngine::Animation animation_;

	//速度
	Vector3 velocity_ = {};

	//コライダー
	std::unique_ptr<DaiEngine::SphereCollider> collider_;

	//生存フラグ
	bool isLife_ = false;
	bool preIsLife_ = false;

	//エフェクト
	std::map <std::string, std::unique_ptr<DaiEngine::GPUParticle>> effect_;
	std::map<std::string, std::unique_ptr<DaiEngine::GPUParticle>> setEff_;
	//焼け跡
	BurnScar* burnScar_ = nullptr;

public:

	~ElementBall();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	void Init(const std::shared_ptr<DaiEngine::Model>& model) override;
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
	/// <summary>
	/// ターゲットセット
	/// </summary>
	/// <param name="target"></param>
	void SetTarget(const Vector3* target) { target_ = target; }
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

	DaiEngine::SphereCollider* GetCollider() { return collider_.get(); }
	/// <summary>
	/// 現在のフェーズ取得
	/// </summary>
	/// <returns>フェーズ</returns>
	Phase GetPhase() const { return phase_; }
	
	void SetBurnScar(BurnScar* burnScar) { burnScar_ = burnScar; }

private:

	void Dead();

};

