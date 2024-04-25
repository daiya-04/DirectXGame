#pragma once
#include "WorldTransform.h"
#include "Camera.h"
#include "Object3d.h"
#include "Vec3.h"
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
	
	enum Parts {
		Body,
		Head,

		partsNum,
	};

	struct WorkAppear{
		Vector3 startPos = { 0.0f,-15.0f,50.0f };
		Vector3 endPos = { 0.0f, 0.0f,50.0f };
		float param = 0.0f;
	};

	struct WorkAttack {
		uint32_t coolTime = 60 * 5;
		uint32_t param = 0;
	};

private:

	std::vector<std::unique_ptr<Object3d>> obj_;
	WorldTransform worldTransform_;
	bool isDead_ = false;

	Vector3 size_ = {};

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

	void Init(const std::vector<std::shared_ptr<Model>>& modelHandle);

	void Update();

	void Draw(const Camera& camera);

	void OnCollision();

	void FloatingGimmickInit();

	void FloatingGimmickUpdate();

	static void SetTarget(const WorldTransform* target) { target_ = target; }
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }
	void ChangeBehavior(Behavior behavior);
	bool IsAttack() const { return (behavior_ == Behavior::kAttack) ? true : false; }

};

