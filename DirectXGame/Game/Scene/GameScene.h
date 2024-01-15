#pragma once
#include "IScene.h"
#include <memory>
#include <list>

#include "Sprite.h"
#include "Object3d.h"
#include "Particle.h"
#include "Camera.h"
#include "WorldTransform.h"

#pragma region

#include "Game/Player/Player.h"
#include "Game/FollowCamera/FollowCamera.h"

#pragma endregion gameObject


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

	FollowCamera camera_;

	uint32_t Model_ = 0;
	uint32_t Model2_ = 0;
	std::unique_ptr<Object3d> obj_;
	std::unique_ptr<Object3d> obj2_;

	std::unique_ptr<Object3d> playerModel_;
	std::unique_ptr<Player> player_;


	WorldTransform objWT_;
	WorldTransform objWT2_;

	std::unique_ptr<Particle> particle_;
	std::list<Particle::ParticleData> particleData_;
	Particle::Emitter emitter_{};
	const float kDeltaTime = 1.0f / 60.0f;

};

