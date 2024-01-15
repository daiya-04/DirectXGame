#pragma once
#include "IScene.h"
#include <memory>
#include <list>

#include "Sprite.h"
#include "Object3d.h"
#include "Particle.h"
#include "Camera.h"
#include "WorldTransform.h"
#include"Quaternion.h"


class SelectScene : public IScene{
public:

	void Init()override;

	void Update()override;

	void SelectStage();

	void DrawBackGround()override;

	void DrawModel()override;

	void DrawParticleModel()override;

	void DrawParticle()override;

	void DrawUI()override;

	void DebugGUI()override;

	~SelectScene()override;

private:
	int selectNum_ = 0;
	int oldSelectNum_ = 0;
	const int maxStage_ = 3;

	const float addEase_ = 0.05f;
	float easeT_ = 0.0f;

	const float addRotateEase_ = 0.2f;
	float easeRotateT_ = 1.0f;

	const float movePos_ = 7.0f;

	float startPos_ = 0.0f;
	float endPos_ = 3.14f;

	float startRotate_ = 3.14f;
	float endRotate_ = 3.14f;

	Camera camera_;

	uint32_t Model_ = 0;
	uint32_t seaHorseModel_ = 0;

	std::unique_ptr<Object3d> obj_;
	std::unique_ptr<Object3d> obj2_;
	std::unique_ptr<Object3d> obj3_;

	std::unique_ptr<Object3d> playerObj_;

	WorldTransform objWT_;
	WorldTransform objWT2_;
	WorldTransform objWT3_;

	WorldTransform playerWT_;
	WorldTransform seaHorseWT_;

	Matrix4x4 rotateMat_;

	Vector3 from_ = { 0.0f,0.0f,1.0f };
	Vector3 rotate_ = { 0.0f,0.0f,1.0f };


	Quaternion quater_;

	Input* input_ = nullptr;
};

