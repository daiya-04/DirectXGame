#pragma once
#include "IScene.h"
#include <memory>
#include <list>
#include <vector>

#include "Sprite.h"
#include "Object3d.h"
#include "Particle.h"
#include "Camera.h"
#include "WorldTransform.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "LevelLoader.h"


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

	Camera camera_;

	PointLight pointLight_;
	SpotLight spotLight_;

	std::unique_ptr<Object3d> cube_;
	std::shared_ptr<Model> cubeModel_;

};

