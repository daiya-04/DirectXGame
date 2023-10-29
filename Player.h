#pragma once
#include "Object3d.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Vec3.h"
#include "Input.h"
#include <vector>
#include <array>


class Player{
private:

	enum Parts {
		Body,
		Head,

		partsNum,
	};

	std::vector<Object3d*> models_;
	WorldTransform worldTransform_;
	std::array<WorldTransform, partsNum> partsWorldTransform_;
	Vector3 size_ = { 1.0f,2.0f,1.0f };

	float speed = 0.5f;
	Vector3 rotate_ = {};
	Vector3 velocity_ = {};

	bool isJamp_ = false;
	bool onGround_ = false;

	const ViewProjection* viewProjection_;

public:

	void Initialize(const std::vector<Object3d*>& models);

	void Update();

	void Draw(const ViewProjection& viewProjection);

	void OnGround();

	void ReStart();

	void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }
	void SetTranslationParent(const WorldTransform* parent) { worldTransform_.translationParent_ = parent; }

	void AddTransform(const Vector3& velocity) { worldTransform_.translation_ += velocity; }

	Vector3 GetWorldPos() const;
	Vector3 GetSize() const { return size_; }
	const WorldTransform& GetWorldTransform() { return worldTransform_; }

};

