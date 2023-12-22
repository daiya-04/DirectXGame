#pragma once
#include "WorldTransform.h"
#include "Camera.h"
#include "Object3d.h"
#include "Vec3.h"
#include "Matrix44.h"
#include <memory>
#include <vector>
#include <array>


class Player{
public:

	enum Parts {
		Body,
		Head,

		kPartsNum,
	};

private:

	std::vector<std::unique_ptr<Object3d>> obj_;
	WorldTransform worldTransform_;
	std::array<WorldTransform, kPartsNum> partsWorldTransform_;
	Vector3 size_ = { 1.0f,2.0f,1.0f };

	Matrix4x4 rotateMat_ = MakeIdentity44();
	Vector3 rotate_ = { 0.0f,0.0f,1.0f };
	Vector3 from_ = { 0.0f,0.0f,1.0f };
	//Vector3 velocity_ = {};

	const Camera* camera_;

public:
	//初期化
	void Init(std::vector<uint32_t> modelHandles);
	//更新
	void Update();
	//描画
	void Draw(const Camera& camera);

	//カメラの設定
	void SetCamera(const Camera* camera) { camera_ = camera; }

	const WorldTransform& GetWorldTransform() { return worldTransform_; }
	Vector3 GetWorldPos() const;

};

