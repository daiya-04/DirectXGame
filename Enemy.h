#pragma once
#include <memory>
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Object3d.h"
#include "Vec3.h"
#include <vector>
#include <array>
#include <optional>


class Enemy{
private:

	enum Parts {
		Body,
		Head,

		partsNum
	};

	std::vector<std::unique_ptr<Object3d>> objects_;
	std::vector<uint32_t> modelHandles_;
	WorldTransform worldTransform_;
	std::array<WorldTransform, partsNum> partsWorldTransform_;
	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };

	

	Vector3 size_ = { 1.0f,1.5f,2.0f };

	float floatingParameter_ = 0.0f;
	// 浮遊の振幅
	float amplitude = 0.5f;
	// 浮遊移動のサイクル<frame>
	int cycle = 60;

	float crushParam_ = 0.0f;

	int life_ = 3;
	bool isDead_ = false;

public:

	void Initialize(std::vector<uint32_t> modelHandles);

	void Update();

	void Draw(const ViewProjection& viewProjection);

	void InitializeFloatingGimmick();

	void UpdateFloatingGimmick();

	void OnCollision();

	void Reset();

	
	void SetPos(const Vector3& position) { worldTransform_.translation_ = position; }
	void SetIsDead(bool isDead) { isDead_ = isDead; }

	Vector3 GetSize() const { return size_; }
	Vector3 GetWorldPos() const;
	bool IsDead() { return isDead_; }

private:

	enum class Behavior {
		kRoot,
		kDead,
	};

	Behavior behavior_ = Behavior::kRoot;
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	static void(Enemy::* BehaviorTable[])();

public:

	//通常行動初期化
	void RootInitialize();
	//通常行動更新
	void RootUpdate();
	//
	void DeadInitialize();
	//
	void DeadUpdate();

};

