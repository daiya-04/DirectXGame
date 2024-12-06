#pragma once
#include "WorldTransform.h"
#include "Camera.h"
#include "Object3d.h"
#include "SkinningObject.h"
#include "SkinCluster.h"
#include "ModelManager.h"
#include "Animation.h"
#include "Vec3.h"
#include "CollisionShapes.h"
#include "LevelLoader.h"
#include "Particle.h"
#include "Sprite.h"
#include "GPUParticle.h"
#include "IcicleManager.h"
#include "PlasmaShotManager.h"
#include "ElementBallManager.h"
#include "GroundFlare.h"
#include "BaseCharactor.h"

#include <memory>
#include <vector>
#include <array>
#include <optional>
#include <functional>
#include <map>

class Boss : public BaseCharactor {
public:
	//行動
	enum Behavior {
		kRoot,
		kAttack,
		kAppear,
		kDead,
	};

	Behavior behavior_ = Behavior::kAppear;
	std::optional<Behavior> behaviorRequest_ = Behavior::kAppear;

	//行動初期化テーブル
	std::map<Behavior, std::function<void()>> behaviorInitTable_{
		{Behavior::kRoot,[this]() {RootInit(); }},
		{Behavior::kAttack,[this]() {AttackInit(); }},
		{Behavior::kAppear,[this]() {AppearInit(); }},
		{Behavior::kDead,[this]() {DeadInit(); }},
	};

	//行動更新テーブル
	std::map<Behavior, std::function<void()>> behaviorUpdateTable_{
		{Behavior::kRoot,[this]() {RootUpdate(); }},
		{Behavior::kAttack,[this]() {AttackUpdate(); }},
		{Behavior::kAppear,[this]() {AppearUpdate(); }},
		{Behavior::kDead,[this]() {DeadUpdate(); }},
	};

private:
	//通常行動初期化
	void RootInit();
	//通常行動更新
	void RootUpdate();
	//登場行動初期化
	void AppearInit();
	//登場行動更新
	void AppearUpdate();
	//攻撃行動初期化
	void AttackInit();
	//攻撃行動更新
	void AttackUpdate();
	//死亡行動初期化
	void DeadInit();
	//死亡行動更新
	void DeadUpdate();

public:
	//アクション(アニメーション)
	enum Action {
		Standing,
		AttackSet,
		Attack,
		Dead,

		ActionNum,
	};

	//攻撃の種類
	enum AttackType {
		kElementBall,
		kGroundFlare,
		kIcicle,
		kPlasmaShot,
	};

private:
	//登場に必要なパラメータ
	struct WorkAppear{
		//開始位置
		Vector3 startPos = { 0.0f,-15.0f,50.0f };
		//終了位置
		Vector3 endPos = { 0.0f, 0.0f,50.0f };
		//時間
		float param = 0.0f;
	};
	//攻撃に必要なパラメータ
	struct WorkAttack {
		//攻撃間隔
		uint32_t coolTime = 60 * 3;
		//時間
		uint32_t param = 0;
	};

private:
	//登場演出
	/*std::unique_ptr<GPUParticle> appearEff_;
	std::unique_ptr<GPUParticle> appearEff2_;*/
	std::map<std::string, std::unique_ptr<GPUParticle>> effect_;
	//攻撃種類
	AttackType attackType_ = AttackType::kElementBall;
	//最大HP
	uint32_t maxHp_ = 15;
	//死亡アニメーション終了フラグ
	bool isFinishDeadMotion_ = false;
	//攻撃のターゲット(プレイヤー)
	const WorldTransform* target_ = nullptr;

	WorkAppear workAppear_;
	WorkAttack workAttack_;

	/// 攻撃の各種マネージャ
	//火の玉
	ElementBallManager* elementBall_ = nullptr;
	//つらら
	IcicleManager* icicle_ = nullptr;
	//電気玉
	PlasmaShotManager* plasmaShot_ = nullptr;
	//地面から炎が噴き出る
	GroundFlare* groundFlare_ = nullptr;
	///

public:

	Boss() {};
	//初期化
	void Init(const std::vector<std::shared_ptr<Model>>& models) override;
	//更新
	void Update() override;
	//UI更新
	void UpdateUI() override;
	//描画(モデル)
	void Draw(const Camera& camera) override;
	//パーティクル描画
	void DrawParticle(const Camera& camera);
	//UI描画
	void DrawUI() override;
	//衝突時
	void OnCollision();
	//ターゲット設定
	void SetTarget(const WorldTransform* target) { target_ = target; }
	//攻撃設定
	void SetElementBall(ElementBallManager* elementBall) { elementBall_ = elementBall; }
	void SetIcicle(IcicleManager* icicle) { icicle_ = icicle; }
	void SetPlasmaShot(PlasmaShotManager* plasmaShot) { plasmaShot_ = plasmaShot; }
	void SetGroudFlare(GroundFlare* groundFlare) { groundFlare_ = groundFlare; }
	//攻撃しているか
	bool IsAttack() const { return (behavior_ == Behavior::kAttack) ? true : false; }
	//死亡アニメーションが終わったか
	bool IsFinishDeadMotion() const { return isFinishDeadMotion_; }

};

