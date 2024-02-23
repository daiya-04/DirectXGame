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

	std::shared_ptr<Model> Model_ = 0;
	std::shared_ptr<Model> Model2_ = 0;
	std::unique_ptr<Object3d> obj_;
	std::unique_ptr<Object3d> obj2_;
	WorldTransform objWT_;
	WorldTransform objWT2_;

	std::unique_ptr<Particle> particle_;
	std::list<Particle::ParticleData> particleData_;
	Particle::Emitter emitter_{};
	const float kDeltaTime = 1.0f / 60.0f;

	std::shared_ptr<Model> terrainModel_ = 0;
	std::unique_ptr<Object3d> terrain_;
	WorldTransform terrainWT_;

	PointLight pointLight_;
	SpotLight spotLight_;

	std::unique_ptr<LevelData> levelData_;
	std::vector<std::unique_ptr<Object3d>> objects_;
	std::vector<WorldTransform> WTs_;

	std::unique_ptr<Sprite> bg_;

};

