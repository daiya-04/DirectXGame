#pragma once
#include "IScene.h"
#include <memory>
#include <list>

#include "Sprite.h"
#include "Object3d.h"
#include "Particle.h"
#include "Camera.h"
#include "WorldTransform.h"

#include "Skydome.h"
#include "Ground.h"
#include "Player.h"
#include "FollowCamera.h"


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

private: //オブジェクト

	std::unique_ptr<Skydome> skydome_;
	std::unique_ptr<Ground> ground_;

	std::unique_ptr<Player> player_;

	std::unique_ptr<FollowCamera> followCamera_;

};

