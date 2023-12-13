#pragma once
#include "IScene.h"
#include <memory>
#include <list>
#include <random>

#include "Sprite.h"
#include "Object3d.h"
#include "Particle.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Quaternion.h"


class GameScene : public IScene {
public:

	void Init()override;

	void Update()override;

	void DrawBackGround()override;

	void DrawModel()override;

	void DrawParticleModel()override;

	void DrawParticle()override;

	void DrawUI()override;

	void DebugGUI()override;

	~GameScene()override;
	

private:

	ViewProjection viewProjection_;

	Quaternion rotation0 = {};
	Quaternion rotation1 = {};

	Quaternion interpolate0 = {};
	Quaternion interpolate1 = {};
	Quaternion interpolate2 = {};
	Quaternion interpolate3 = {};
	Quaternion interpolate4 = {};

};

