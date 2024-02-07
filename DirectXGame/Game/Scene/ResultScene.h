#pragma once
#include "IScene.h"
#include <memory>
#include <list>
#include <random>

#include "Sprite.h"
#include "Object3d.h"
#include "Particle.h"
#include "Camera.h"
#include "WorldTransform.h"
#include"TimeCounter/TimeCounter.h"

class ResultScene : public IScene{
public:

	void Init()override;

	void Update()override;

	void DrawBackGround()override;

	void DrawModel()override;

	void DrawParticleModel()override;

	void DrawParticle()override;

	void DrawUI()override;

	void DebugGUI()override;

	~ResultScene()override;

private:

	std::unique_ptr<Sprite> background_;

	std::unique_ptr<Sprite> title_;

	std::unique_ptr<Sprite> press_;

	std::unique_ptr<TimeCounter> timeCounter_;

	Vector2 scale_{};

	Vector2 pos_{};

	float timenum_;

	WorldTransform titleTrnas_;

	WorldTransform pressTrnas_;

	bool isNext_ = false;

	Input* input_ = nullptr;

	//音
	size_t SEHandle_;

};

