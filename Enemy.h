#pragma once
#include <memory>
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Object3d.h"
#include "Vec3.h"
#include <vector>
#include <array>


class Enemy{
private:

	enum Parts {
		Body,
		Head,

		partsNum
	};

	std::vector<std::unique_ptr<Object3d>> objects_;
	WorldTransform worldTransform_;
	std::array<WorldTransform, partsNum> partsWorldTransform_;

	Vector3 center_;
	float radius_ = 10.0f;
	float theta_ = 0.0f;

	Vector3 size_ = { 1.0f,1.5f,2.0f };

	float floatingParameter_ = 0.0f;
	// 浮遊の振幅
	float amplitude = 0.5f;
	// 浮遊移動のサイクル<frame>
	int cycle = 60;

	bool isDead_ = false;
	int rePopTime = 60;
	int rePopCount_ = 0;

public:

	void Initialize(std::vector<uint32_t> modelHandles);

	void Update();

	void Draw(const ViewProjection& viewProjection);

	void InitializeFloatingGimmick();

	void UpdateFloatingGimmick();

	void OnCollision();

	void Reset();

	void SetCenter(const Vector3& center) { center_ = center; }
	void SetPos(const Vector3& position) { worldTransform_.translation_ = position; }

	Vector3 GetSize() const { return size_; }
	Vector3 GetWorldPos() const;

};

