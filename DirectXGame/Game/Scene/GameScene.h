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
#include "Enemy.h"
#include "EnemyBullet.h"
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

	GameScene() :randomEngine(seedGenerator()) {}

	Enemy* EnemyPop(std::vector<uint32_t> modelHandles, Vector3 pos);
	
	void AddEnemyBullet(EnemyBullet* enemyBullet);

	void AddParticle(Particle::ParticleData particle);

private:

	Camera camera_;

private: //オブジェクト

	std::unique_ptr<Skydome> skydome_;
	std::unique_ptr<Ground> ground_;

	std::unique_ptr<Player> player_;
	std::list<std::unique_ptr<Enemy>> enemies_;
	std::list<std::unique_ptr<EnemyBullet>> enemyBullets_;

	std::unique_ptr<FollowCamera> followCamera_;

	std::unique_ptr<Particle> particle_;
	std::list<Particle::ParticleData> particles_;

	uint32_t enemyBodyModel_ = 0;
	uint32_t enemyHeadModel_ = 0;
	uint32_t bulletModel_ = 0;

	uint32_t gameTime_ = 60 * 60;
	uint32_t gameCount_ = gameTime_;

	uint32_t finishTime_ = 60 * 5;
	uint32_t finishCount_ = finishTime_;

	uint32_t spawnCoolTime_ = 60 * 10;
	uint32_t spawnCount_ = spawnCoolTime_;
	uint32_t spawnNum_ = 30;

	std::random_device seedGenerator;
	std::mt19937 randomEngine;

	std::unique_ptr<Sprite> XButton_;
	std::unique_ptr<Sprite> char_Attack_;
	std::unique_ptr<Sprite> finish_;

	Vector2 pos1 = {};
	Vector2 pos2 = {};

};

