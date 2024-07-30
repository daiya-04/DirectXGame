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
#include "GPUParticle.h"
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
#include "PlayerAttack.h"
#include "BurnScars.h"
#include "GroundFlare.h"


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

	void AddPlayerAttack(PlayerAttack* playerAttack);

	void CreateBurnScars(const Vector3& createPos);

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
	std::list<std::unique_ptr<PlayerAttack>> playerAttacks_;

	std::unique_ptr<FollowCamera> followCamera_;

	
	std::shared_ptr<Model> enemyBodyModel_ = 0;
	std::shared_ptr<Model> enemyHeadModel_ = 0;
	std::shared_ptr<Model> bulletModel_ = 0;

	uint32_t finishTime_ = 60 * 3;
	uint32_t finishCount_ = finishTime_;

	std::unique_ptr<Sprite> XButton_;
	std::unique_ptr<Sprite> char_Attack_;
	std::unique_ptr<Sprite> gameOver_;
	std::unique_ptr<Sprite> finish_;

	float alpha_ = 0.0f;

	Vector2 pos1 = {};
	Vector2 pos2 = {};

	PostEffect* postEffect_;
	OutLine* outLine_;

	std::unique_ptr<LevelData> levelData_;

	uint32_t burnScarsTex_ = 0;
	std::list<std::unique_ptr<BurnScars>> burnScarses_;

	GroundFlare* groundFlare_ = nullptr;


	bool isGameStop_ = false;

private:

	enum class SceneEvent {
		Battle,
		PlayerDead,
		Clear,
	};

private:

	void BattleInit();

	void BattleUpdate();

	void PlayerDeadInit();

	void PlayerDeadUpdate();

	void ClearInit();

	void ClearUpdate();

private:

	SceneEvent sceneEvent_ = SceneEvent::Battle;
	std::optional<SceneEvent> eventRequest_ = SceneEvent::Battle;

};

