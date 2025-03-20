#pragma once
#include "BaseAttack.h"
#include "WorldTransform.h"
#include "GPUParticle.h"
#include "SphereCollider.h"
#include "IceScar.h"

#include <optional>
#include <functional>
#include <map>

class IceImpact : public BaseAttack {
public:

	enum class Phase {
		//通常
		kIdle,
		//溜め
		kCharge,
		//待ち
		kWait,
		//爆発
		kImpact,
	};

	//現在のフェーズ
	Phase phase_ = Phase::kIdle;
	//次のフェーズのリクエスト
	std::optional<Phase> phaseRequest_ = Phase::kIdle;
	//フェーズ初期化テーブル
	std::map<Phase, std::function<void()>> phaseInitTable_{
		{Phase::kIdle,[this]() {IdleInit(); }},
		{Phase::kCharge,[this]() {ChargeInit(); }},
		{Phase::kWait,[this]() {WaitInit(); }},
		{Phase::kImpact,[this]() {ImpactInit(); }},
	};
	//フェーズ更新テーブル
	std::map<Phase, std::function<void()>> phaseUpdateTable_{
		{Phase::kIdle,[this]() {IdleUpdate(); }},
		{Phase::kCharge,[this]() {ChargeUpdate(); }},
		{Phase::kWait,[this]() {WaitUpdate(); }},
		{Phase::kImpact,[this]() {ImpactUpdate(); }},
	};

private:

	void IdleInit();
	void IdleUpdate();

	void ChargeInit();
	void ChargeUpdate();

	void WaitInit();
	void WaitUpdate();

	void ImpactInit();
	void ImpactUpdate();

private:

	DaiEngine::WorldTransform worldTrabsform_;
	//コライダー
	std::unique_ptr<DaiEngine::SphereCollider> collider_;
	//氷の跡
	IceScar* iceScar_ = nullptr;
	//エフェクト
	std::map<std::string, std::unique_ptr<DaiEngine::GPUParticle>> setEff_;
	std::map<std::string, std::unique_ptr<DaiEngine::GPUParticle>> impactEff_;
	//カウンター
	uint32_t counter_ = 0;
	//溜めの時間
	uint32_t chargeTime_ = 120;
	//待ちの時間
	uint32_t waitTime_ = 60;
	//爆発の時間
	uint32_t impactTime_ = 60;

	bool isAttack_ = false;

public:

	~IceImpact();
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
	/// <summary>
	/// 攻撃開始
	/// </summary>
	void AttackStart();
	/// <summary>
	/// 攻撃に必要なデータのセット
	/// </summary>
	/// <param name="pos"></param>
	void SetAttackData(const Vector3& pos);
	/// <summary>
	/// 攻撃中か
	/// </summary>
	/// <returns></returns>
	bool IsAttack() const { return isAttack_; }

	void SetIceScar(IceScar* iceScar) { iceScar_ = iceScar; }

};

