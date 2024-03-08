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

class Enemy{
public:

	enum Parts {
		Body,
		Head,

		partsNum,
	};

private:

	std::vector<std::unique_ptr<Object3d>> obj_;
	WorldTransform worldTransform_;
	std::array<WorldTransform, partsNum> partsWorldTransform_;
	bool isDead_ = false;

	Vector3 size_ = { 1.0f,1.5f,2.0f };

	static const WorldTransform* target_;

	Matrix4x4 rotateMat_ = MakeIdentity44();

	uint32_t coolTime_ = 0;
	uint32_t attackTimer_ = 0;

	std::random_device seedGenerator;
	std::mt19937 randomEngine;

	GameScene* gameScene_ = nullptr;

	float floatingParameter_ = 0.0f;
	// 浮遊の振幅
	float amplitude = 0.5f;
	// 浮遊移動のサイクル<frame>
	int cycle = 60;

public:

	Enemy() :randomEngine(seedGenerator()) {}

	//初期化
	void Init(std::vector<std::shared_ptr<Model>> modelHandles);
	//更新
	void Update();
	//描画
	void Draw(const Camera& camera);

	void OnCollision();

	void FloatingGimmickInit();

	void FloatingGimmickUpdate();

	void SetPos(const Vector3& pos) { worldTransform_.translation_ = pos; }

	void SetCoolTime(uint32_t coolTime);

	static void SetTarget(const WorldTransform* target) { target_ = target; }

	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	const WorldTransform& GetWorldTransform() { return worldTransform_; }
	Vector3 GetWorldPos() const;
	Vector3 GetSize() const { return size_; }
	bool IsDead() { return isDead_; }
};

