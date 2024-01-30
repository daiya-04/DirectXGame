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

#pragma region
#include "Game/FollowCamera/FollowCamera.h"
#include "Game/Player/Player.h"
#include "Game/Floor/Floor.h"
#include "Game/Sango/Sango.h"
#include "Game/Box/Box.h"

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

	std::unique_ptr<Object3d> playerModel_;
	std::unique_ptr<Object3d> arrowModel_;
	std::unique_ptr<Player> player_;

	std::unique_ptr<Object3d> floorModel_;
	std::unique_ptr<Floor> floor_;

	std::unique_ptr<Object3d> sangoModel_;

	std::unique_ptr<Sango> sango_;
	std::unique_ptr<Sango> sango2_;
	std::unique_ptr<Sango> sango3_;
	std::unique_ptr<Sango> sango4_;

	std::list<Box*>box_;
};

