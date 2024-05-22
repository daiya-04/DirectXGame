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
#include <memory>
#include <vector>
#include <array>
#include <optional>
#include "Particle.h"

class GameScene;

class Boss{
public:

	enum Behavior {
		kRoot,
		kAttack,
		kAppear,
	};

	Behavior behavior_ = Behavior::kRoot;
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

private:

	void RootInit();
	void RootUpdate();
	void AppearInit();
	void AppearUpdate();
	void AttackInit();
	void AttackUpdate();

public:
	
	enum Action {
		Standing,
		AttackSet,
		Attack,

		ActionNum,
	};

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

	std::vector<std::unique_ptr<Object3d>> debugObj_;
	std::shared_ptr<Model> debugModel_;

	Action action_ = Action::Standing;


	bool isDead_ = false;

	Vector3 size_ = { 2.0f,5.0f,2.0f };

	Shapes::AABB collider_{};

	static const WorldTransform* target_;
	GameScene* gameScene_ = nullptr;

	Matrix4x4 rotateMat_ = MakeIdentity44();

	WorkAppear workAppear_;
	WorkAttack workAttack_;

	uint32_t coolTime_ = 0;
	uint32_t attackTimer_ = 0;

	float floatingParameter_ = 0.0f;
	// 浮遊の振幅
	float amplitude = 0.3f;
	// 浮遊移動のサイクル<frame>
	int cycle = 60;

public:

	Boss() {};

	void Init(const std::vector<std::shared_ptr<Model>>& models);

	void Update();
	
	void ColliderUpdate() {
		collider_.max = GetWorldPos() + size_;
		collider_.min = GetWorldPos() - size_;
	}

	void Draw(const Camera& camera);
	void SkeletonDraw(const Camera& camera);

	void OnCollision();

	static void SetTarget(const WorldTransform* target) { target_ = target; }
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }
	void ChangeBehavior(Behavior behavior);
	void ChangeAction(Action action) {
		action_ = action;
		animations_[action_].Play(skeletons_[action_]);
	}
	bool IsAttack() const { return (behavior_ == Behavior::kAttack) ? true : false; }
	Vector3 GetWorldPos() const;
	Shapes::AABB GetCollider(){ return collider_; }
	Action GetAction() const { return action_; }

};

