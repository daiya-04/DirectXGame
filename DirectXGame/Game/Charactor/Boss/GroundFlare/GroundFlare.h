#pragma once
///---------------------------------------------------------------------------------------------
// 
// 地面から陽が噴き出る攻撃
// ボスの攻撃の一種
// 
///---------------------------------------------------------------------------------------------

#include "BaseAttack.h"
#include "WorldTransform.h"
#include "GPUParticle.h"
#include "CollisionShapes.h"
#include "OBBCollider.h"
#include "BurnScar.h"

#include <array>
#include <optional>
#include <map>
#include <functional>

//地面から炎攻撃のクラス
class GroundFlare : public BaseAttack {
public:
	//フェーズ
	enum class Phase {
		//通常
		kRoot,
		//待ち
		kWait,
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
		{Phase::kRoot, [this]() { RootInit(); }},
		{Phase::kWait, [this]() { WaitInit(); }},
		{Phase::kWarning, [this]() { WarningInit(); }},
		{Phase::kFire, [this]() { FireInit(); }},
	};
	//フェーズ更新テーブル
	std::map<Phase, std::function<void()>> phaseUpdateTable_{
		{Phase::kRoot, [this]() { RootUpdate(); }},
		{Phase::kWait, [this]() { WaitUpdate(); }},
		{Phase::kWarning, [this]() { WarningUpdate(); }},
		{Phase::kFire, [this]() { FireUpdate(); }},
	};

private:
	//待ちに必要なパラメータ
	struct WorkWait {
		int32_t param_ = 0;
		int32_t waitTime_ = 0;
	};

	//警告地点に必要なパラメータ
	struct WorkWarning {
		//イージング用
		float param_ = 0.0f;
		//円が広がるスピード
		float speed_ = 0.025f;
		//開始スケール
		Vector3 startScale_ = { 0.0f, 1.0f, 0.0f };
		//終了スケール
		Vector3 endScale_ = { 1.3f, 1.0f, 1.3f };
	};
	//炎の発射に必要なパラメータ
	struct WorkFire {
		//発射時間
		int32_t fireCount_ = 50;
		//発射中の現在の時間
		int32_t param_ = 0;
	};

	//待ち用パラメータ
	WorkWait workWait_;
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
	/// 待ち初期化
	/// </summary>
	void WaitInit();
	/// <summary>
	/// 待ち更新
	/// </summary>
	void WaitUpdate();
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

	~GroundFlare();
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	void Init() override;
	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(const DaiEngine::Camera& camera) override;
	/// <summary>
	/// パーティクル描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void DrawParticle(const DaiEngine::Camera& camera) override;
	/// <summary>
	/// 衝突時
	/// </summary>
	/// <param name="other"></param>
	void OnCollision(DaiEngine::Collider* other) override;
	/// <summary>
	/// 攻撃開始
	/// </summary>
	void AttackStart(const Vector3& firePos, int32_t interval);
	/// <summary>
	/// 発射中か
	/// </summary>
	/// <returns></returns>
	bool IsFire() const { return isFire_; }
	/// <summary>
	/// 攻撃中か
	/// </summary>
	/// <returns></returns>
	bool IsAttack() const { return isAttack_; }

	void SetBurnScar(BurnScar* burnScar) { burnScar_ = burnScar; }

private:

	DaiEngine::WorldTransform worldTransform_;
	//コライダー
	std::unique_ptr<DaiEngine::OBBCollider> collider_;
	
	//焼け跡
	BurnScar* burnScar_ = nullptr;
	//跡の高さ調整用
	static size_t heightAdjustmentIndex_;

	//エフェクト
	std::map<std::string, std::unique_ptr<DaiEngine::GPUParticle>> fire_;

	//警告の円
	std::unique_ptr<DaiEngine::Object3d> warningZone_;
	//攻撃中か
	bool isAttack_ = false;
	//発射中か
	bool isFire_ = false;

};

