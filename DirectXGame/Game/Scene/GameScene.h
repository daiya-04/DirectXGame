#pragma once
#include "IScene.h"
#include <memory>
#include <list>
#include <vector>
#include <optional>

#include "Sprite.h"
#include "Object3d.h"
#include "SkinningObject.h"
#include "Particle.h"
#include "Camera.h"
#include "WorldTransform.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "LevelLoader.h"
#include "PostEffect.h"
#include "OutLine.h"

#include "Skydome.h"
#include "Ground.h"
#include "Player.h"
#include "Enemy.h"
#include "EnemyBullet.h"
#include "Boss.h"
#include "FollowCamera.h"
#include "ElementBall.h"


class GameScene : public IScene {
public:

	void Init()override;

	void Update()override;

	void DrawBackGround()override;

	void DrawModel()override;

	void DrawParticleModel()override;

	void DrawParticle()override;

	void DrawUI()override;

	void DrawPostEffect()override;

	void DrawRenderTexture()override;

	void DebugGUI()override;

	~GameScene()override;

	GameScene();

	//Enemy* EnemyPop(std::vector<std::shared_ptr<Model>> modelHandles, Vector3 pos);
	
	//void AddEnemyBullet(EnemyBullet* enemyBullet);

	void AddElementBall(ElementBall* elementBall);

	void AddParticle(Particle::ParticleData particle);

private:
  
	Camera camera_;

	PointLight pointLight_;
	SpotLight spotLight_;

private: //オブジェクト

	std::unique_ptr<Skydome> skydome_;
	std::unique_ptr<Ground> ground_;

	std::unique_ptr<Player> player_;
	/*std::list<std::unique_ptr<Enemy>> enemies_;
	std::list<std::unique_ptr<EnemyBullet>> enemyBullets_;*/
	std::unique_ptr<Boss> boss_;

	std::list<std::unique_ptr<ElementBall>> elementBalls_;

	std::unique_ptr<FollowCamera> followCamera_;

	std::unique_ptr<Particle> particle_;
	std::list<Particle::ParticleData> particles_;

	std::shared_ptr<Model> enemyBodyModel_ = 0;
	std::shared_ptr<Model> enemyHeadModel_ = 0;
	std::shared_ptr<Model> bulletModel_ = 0;

	uint32_t finishTime_ = 60 * 3;
	uint32_t finishCount_ = finishTime_;

	uint32_t spawnCoolTime_ = 60 * 10;
	uint32_t spawnCount_ = spawnCoolTime_;
	uint32_t spawnNum_ = 30;

	std::random_device seedGenerator;
	std::mt19937 randomEngine;

	std::unique_ptr<Sprite> XButton_;
	std::unique_ptr<Sprite> char_Attack_;
	std::unique_ptr<Sprite> gameOver_;

	float alpha_ = 0.0f;

	Vector2 pos1 = {};
	Vector2 pos2 = {};

	PostEffect* postEffect_;
	OutLine* outLine_;

	std::unique_ptr<LevelData> levelData_;

private:

	enum class SceneEvent {
		Battle,
		PlayerDead,
	};

private:

	void BattleInit();

	void BattleUpdate();

	void PlayerDeadInit();

	void PlayerDeadUpdate();

private:

	SceneEvent sceneEvent_ = SceneEvent::Battle;
	std::optional<SceneEvent> eventRequest_ = SceneEvent::Battle;

};

