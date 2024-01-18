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

class TitleScene: public IScene{
public:

	void Init()override;

	void Update()override;

	void DrawBackGround()override;

	void DrawModel()override;

	void DrawParticleModel()override;

	void DrawParticle()override;

	void DrawUI()override;

	void DebugGUI()override;

	~TitleScene()override;

private:


	Camera camera_;
	std::unique_ptr<Particle> particle_;
	std::list<Particle::ParticleData> particles_;
	Particle::Emitter emitter_{};
	const float kDeltaTime = 1.0f / 60.0f;

	bool isField_ = false;

	Particle::AccelerationField accelerationField_;

	Input* input_ = nullptr;

};

