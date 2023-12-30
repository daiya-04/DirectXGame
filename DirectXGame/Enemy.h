#pragma once
#include "WorldTransform.h"
#include "Camera.h"
#include "Object3d.h"
#include "Vec3.h"
#include <memory>
#include <vector>
#include <array>
#include <optional>

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

	Vector3 size_ = { 1.0f,1.5f,2.0f };

public:

	//初期化
	void Init(std::vector<uint32_t> modelHandles);
	//更新
	void Update();
	//描画
	void Draw(const Camera& camera);

	void SetPos(const Vector3& pos) { worldTransform_.translation_ = pos; }

};

