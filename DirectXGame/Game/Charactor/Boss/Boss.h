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

#include <memory>
#include <vector>
#include <array>
#include <optional>
#include <functional>
#include <map>

class Boss{
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

	std::unique_ptr<SkinningObject> obj_;
	std::vector<std::shared_ptr<Model>> animationModels_;
	std::vector<Animation> animations_;
	std::vector<Skeleton> skeletons_;
	std::vector<SkinCluster> skinClusters_;

	std::unique_ptr<GPUParticle> appearEff_;
	std::unique_ptr<GPUParticle> appearEff2_;

	Action action_ = Action::Standing;
	AttackType attackType_ = AttackType::kElementBall;

	uint32_t maxHp_ = 10;
	uint32_t life_ = maxHp_;

	bool isDead_ = false;
	bool isFinishDeadMotion_ = false;

	Vector3 size_{};

	Shapes::AABB collider_{};

	const WorldTransform* target_ = nullptr;

	Vector3 direction_ = { 0.0f,0.0f,-1.0f };
	Matrix4x4 rotateMat_ = MakeIdentity44();

	WorkAppear workAppear_;
	WorkAttack workAttack_;

	uint32_t coolTime_ = 0;
	uint32_t attackTimer_ = 0;

	uint32_t hpBerTex_ = 0;
	std::unique_ptr<Sprite> hpBer_;

public:

	Boss() {};

	void Init(const std::vector<std::shared_ptr<Model>>& models);

	void Update();
	
	void ColliderUpdate() {
		collider_.max = GetWorldPos() + size_;
		collider_.min = GetWorldPos() - size_;
	}

	void UIUpdate();

	void Draw(const Camera& camera);

	void DrawParticle(const Camera& camera);

	void DrawUI();

	void OnCollision();

	void SetData(const LevelData::ObjectData& data);

	void SetTarget(const WorldTransform* target) { target_ = target; }

	void ChangeBehavior(Behavior behavior);
	void ChangeAction(Action action) {
		action_ = action;
		animations_[action_].Play(skeletons_[action_]);
	}
	bool IsAttack() const { return (behavior_ == Behavior::kAttack) ? true : false; }
	bool IsDead() const { return isDead_; }
	bool IsFinishDeadMotion() const { return isFinishDeadMotion_; }
	Vector3 GetWorldPos() const;
	Shapes::AABB GetCollider(){ return collider_; }
	Action GetAction() const { return action_; }
	const WorldTransform& GetWorldTransform() { return obj_->worldTransform_; }

};

