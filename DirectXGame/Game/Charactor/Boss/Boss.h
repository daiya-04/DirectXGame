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

	enum Behavior {
		kRoot,
		kAttack,
		kAppear,
		kDead,
	};

	Behavior behavior_ = Behavior::kAppear;
	std::optional<Behavior> behaviorRequest_ = Behavior::kAppear;

	std::map<Behavior, std::function<void()>> behaviorInitTable_{
		{Behavior::kRoot,[this]() {RootInit(); }},
		{Behavior::kAttack,[this]() {AttackInit(); }},
		{Behavior::kAppear,[this]() {AppearInit(); }},
		{Behavior::kDead,[this]() {DeadInit(); }},
	};

	std::map<Behavior, std::function<void()>> behaviorUpdateTable_{
		{Behavior::kRoot,[this]() {RootUpdate(); }},
		{Behavior::kAttack,[this]() {AttackUpdate(); }},
		{Behavior::kAppear,[this]() {AppearUpdate(); }},
		{Behavior::kDead,[this]() {DeadUpdate(); }},
	};

private:

	void RootInit();
	void RootUpdate();
	void AppearInit();
	void AppearUpdate();
	void AttackInit();
	void AttackUpdate();
	void DeadInit();
	void DeadUpdate();

public:
	
	enum Action {
		Standing,
		AttackSet,
		Attack,
		Dead,

		ActionNum,
	};

	enum AttackType {
		kElementBall,
		kGroundFlare,
		kIcicle,
		kPlasmaShot,
	};

private:

	struct WorkAppear{
		Vector3 startPos = { 0.0f,-15.0f,50.0f };
		Vector3 endPos = { 0.0f, 0.0f,50.0f };
		float param = 0.0f;
	};

	struct WorkAttack {
		uint32_t coolTime = 60 * 3;
		uint32_t param = 0;
	};

private:

	std::unique_ptr<GPUParticle> appearEff_;
	std::unique_ptr<GPUParticle> appearEff2_;

	AttackType attackType_ = AttackType::kElementBall;

	uint32_t maxHp_ = 15;

	bool isFinishDeadMotion_ = false;

	const WorldTransform* target_ = nullptr;

	WorkAppear workAppear_;
	WorkAttack workAttack_;

	uint32_t coolTime_ = 0;
	uint32_t attackTimer_ = 0;

	ElementBallManager* elementBall_ = nullptr;
	IcicleManager* icicle_ = nullptr;
	PlasmaShotManager* plasmaShot_ = nullptr;
	GroundFlare* groundFlare_ = nullptr;

public:

	Boss() {};

	void Init(const std::vector<std::shared_ptr<Model>>& models) override;

	void Update() override;

	void UpdateUI() override;

	void Draw(const Camera& camera) override;

	void DrawParticle(const Camera& camera);

	void DrawUI() override;

	void OnCollision();

	void SetTarget(const WorldTransform* target) { target_ = target; }

	void SetElementBall(ElementBallManager* elementBall) { elementBall_ = elementBall; }
	void SetIcicle(IcicleManager* icicle) { icicle_ = icicle; }
	void SetPlasmaShot(PlasmaShotManager* plasmaShot) { plasmaShot_ = plasmaShot; }
	void SetGroudFlare(GroundFlare* groundFlare) { groundFlare_ = groundFlare; }

	bool IsAttack() const { return (behavior_ == Behavior::kAttack) ? true : false; }
	bool IsFinishDeadMotion() const { return isFinishDeadMotion_; }

};

