#pragma once
#include "IScene.h"
#include <memory>
#include <list>

#include "Sprite.h"
#include "Object3d.h"
#include "Particle.h"
#include "Camera.h"
#include "WorldTransform.h"


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

	uint32_t Model_ = 0;
	uint32_t Model2_ = 0;
	std::unique_ptr<Object3d> obj_;
	std::unique_ptr<Object3d> obj2_;
	WorldTransform objWT_;
	WorldTransform objWT2_;

	std::unique_ptr<Particle> particle_;
	std::list<Particle::ParticleData> particleData_;
	Particle::Emitter emitter_{};
	const float kDeltaTime = 1.0f / 60.0f;

	size_t se1 = 0;
	size_t se2 = 0;
	size_t se3 = 0;

};

