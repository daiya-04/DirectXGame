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

	const float addEase_ = 0.04f;
	float easeT_ = 0.0f;

	const float addRotateEase_ = 0.2f;
	float easeRotateT_ = 1.0f;

	const float movePos_ = 7.0f;

	float startPos_ = 0.0f;
	float endPos_ = 3.14f;

	float startPlayerPos_ = 0.0f;
	float endPlayerPos_ = 3.14f;

	float startRotate_ = 3.14f;
	float endRotate_ = 3.14f;

	Camera camera_;

	uint32_t Model_ = 0;
	uint32_t seaHorseModel_ = 0;
	uint32_t floorModel_ = 0;
	uint32_t rockModel_ = 0;
	uint32_t stageSelectModel_ = 0;
	uint32_t skyModel_ = 0;

	uint32_t loadModel_ = 0;

	std::unique_ptr<Object3d> obj_;
	std::unique_ptr<Object3d> obj2_;
	std::unique_ptr<Object3d> obj3_;

	std::unique_ptr<Object3d> loadObj_;

	std::unique_ptr<Object3d> floorObj_;
	std::unique_ptr<Object3d> rockObj_[3];
	std::unique_ptr<Object3d> stageSelectObj_[3];

	std::unique_ptr<Object3d> playerObj_;

	std::unique_ptr<Object3d> skyDomeObj_;

	WorldTransform objWT_;
	WorldTransform objWT2_;
	WorldTransform objWT3_;

	WorldTransform loadWT_;

	WorldTransform floorWT_;
	WorldTransform rockWT_[3];
	WorldTransform stageSelectWT_[3];

	WorldTransform playerWT_;
	WorldTransform seaHorseWT_;

	WorldTransform skyDomeWT_;

	Matrix4x4 rotateMat_;

	Vector3 from_ = { 0.0f,0.0f,1.0f };
	Vector3 rotate_ = { 0.0f,0.0f,1.0f };


	Quaternion quater_;

	Input* input_ = nullptr;
};

