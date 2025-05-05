#pragma once
#include "BaseAttack.h"
#include "WorldTransform.h"
#include "Camera.h"
#include "GPUParticle.h"
#include "OBBCollider.h"
#include "Vec3.h"

#include <memory>
#include <optional>
#include <map>
#include <functional>


class GroundBurst : public BaseAttack {
public:

	enum class Phase {
		kIdle,
		kAttack,
	};

	Phase phase_ = Phase::kIdle;
	std::optional<Phase> phaseRequest_ = Phase::kIdle;

	std::map<Phase, std::function<void()>> phaseInitTable_{
		{Phase::kIdle, [this]() { IdleInit(); }},
		{Phase::kAttack, [this]() {AttackInit(); }},
	};

	std::map <Phase, std::function<void()>> phaseUpdateTable_{
		{Phase::kIdle, [this]() {IdleUpdate(); }},
		{Phase::kAttack, [this]() {AttackUpdate(); }},
	};


private:
	/// <summary>
	/// 通常
	/// </summary>
	void IdleInit();
	void IdleUpdate();
	/// <summary>
	/// 攻撃
	/// </summary>
	void AttackInit();
	void AttackUpdate();

public:

	~GroundBurst();
	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override;
	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;
	/// <summary>
	/// Particle描画
	/// </summary>
	/// <param name="camera"></param>
	void DrawParticle(const DaiEngine::Camera& camera) override;
	/// <summary>
	/// 衝突のお知らせ
	/// </summary>
	void OnCollision(DaiEngine::Collider* other) override;
	/// <summary>
	/// 攻撃開始
	/// </summary>
	void AttackStart(const Vector3& attackPos);
	/// <summary>
	/// コライダー取得
	/// </summary>
	/// <returns></returns>
	DaiEngine::OBBCollider* GetCollider() const { return collider_.get(); }

	bool IsAttack() const { return isAttack_; }

private:

	void Dead();

private:
	//エフェクト
	std::map<std::string, std::unique_ptr<DaiEngine::GPUParticle>> effect_;

	//コライダー
	std::unique_ptr<DaiEngine::OBBCollider> collider_;
	Matrix4x4 rotateMat_ = MakeIdentity44();

	DaiEngine::WorldTransform worldTransform_;

	bool isAttack_ = false;

	//有効射程
	float firingRange_ = 15.0f;

	uint32_t attackTime_ = 30;
	uint32_t count_ = 0;

	int32_t damage_ = 3;

};

