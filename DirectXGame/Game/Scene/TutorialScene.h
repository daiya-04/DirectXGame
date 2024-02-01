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

class TutorialScene: public IScene{
public:

	void Init()override;

	void Update()override;

	void DrawBackGround()override;

	void DrawModel()override;

	void DrawParticleModel()override;

	void DrawParticle()override;

	void DrawUI()override;

	void DebugGUI()override;

	~TutorialScene()override;

private:

	std::unique_ptr<Sprite> background_;

	std::unique_ptr<Sprite> title_;

	std::unique_ptr<Sprite> press_;

	WorldTransform titleTrnas_;

	WorldTransform pressTrnas_;

	bool isNext_ = false;

	Input* input_ = nullptr;

};

