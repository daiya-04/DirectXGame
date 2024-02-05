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

#include "SceneManager.h"

#include "TimeCounter/TimeCounter.h"

#pragma region
#include "Game/FollowCamera/FollowCamera.h"
#include "Game/Player/Player.h"
#include "Game/Floor/Floor.h"
#include "Game/Sango/Sango.h"
#include "Game/Box/Box.h"
#include "Game/Goal/Goal.h"

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

	/*std::unique_ptr<Object3d> floorModel_;
	std::unique_ptr<Floor> floor_;*/

	std::unique_ptr<Object3d> sangoModel_;

	std::vector<std::unique_ptr<Sango>> sangoes_;

	std::unique_ptr<Object3d> goal_Model_;
	std::unique_ptr<Goal> goal_;
	bool IsGoal = false;

	std::unique_ptr<Object3d> box_Model_;
	std::vector<std::unique_ptr<Box>>boxes_;

	std::unique_ptr<LevelData> levelData_;

	std::unique_ptr<TimeCounter> timeCounter_;

	std::unique_ptr<Sprite> UI_Grap;
	std::unique_ptr<Sprite> UI_PlayerRoring;
	WorldTransform world_;
};

