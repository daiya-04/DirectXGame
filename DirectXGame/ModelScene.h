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

class ModelScene : public IScene {
public:

	void Init()override;

	void Update()override;

	void DrawBackGround()override;

	void DrawModel()override;

	void DrawParticleModel()override;

	void DrawParticle()override;

	void DrawUI()override;

	void DebugGUI()override;

	~ModelScene()override;


private:

	Camera camera_;

	PointLight pointLight_;
	SpotLight spotLight_;

	std::unique_ptr<LevelData> levelData_;
	std::vector<std::unique_ptr<Object3d>> teapots_;
	std::vector<WorldTransform> teapotWTs_;

	std::unique_ptr<Object3d> terrain_;
	WorldTransform terrainWT_;

	const float kDeltaTime = 1.0f / 60.0f;
	std::unique_ptr<Particle> particle1_;
	std::list<Particle::ParticleData> particleData1_;
	Particle::Emitter emitter1_{};

	size_t bgmHandle_ = 0;
	size_t playerHandle_ = 0;

};

