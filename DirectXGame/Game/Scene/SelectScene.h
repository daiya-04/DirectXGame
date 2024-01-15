#pragma once
#include "IScene.h"
#include <memory>
#include <list>

#include "Sprite.h"
#include "Object3d.h"
#include "Particle.h"
#include "Camera.h"
#include "WorldTransform.h"
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

	const float movePos_ = 7.0f;

	float startPos_ = 0.0f;
	float endPos_ = 0.0f;

	Camera camera_;

	uint32_t Model_ = 0;
	std::unique_ptr<Object3d> obj_;
	std::unique_ptr<Object3d> obj2_;
	std::unique_ptr<Object3d> obj3_;
	WorldTransform objWT_;
	WorldTransform objWT2_;
	WorldTransform objWT3_;

	Input* input_ = nullptr;
};

