#pragma once
#include "IScene.h"
#include <memory>
#include <list>
#include <vector>
#include <optional>
#include <map>
#include <functional>

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
#include "HSVFilter.h"
#include "SkyBox.h"

#include "Ground.h"
#include "Player.h"
#include "Boss.h"
#include "FollowCamera.h"
#include "ElementBallManager.h"
#include "PlayerAttack.h"
#include "BurnScars.h"
#include "GroundFlare.h"
#include "IcicleManager.h"
#include "PlasmaShotManager.h"
#include "Rock.h"


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

	void AddPlayerAttack(PlayerAttack* playerAttack);

private:
  
	Camera camera_;

	PointLight pointLight_;
	SpotLight spotLight_;

private: //オブジェクト

	
	std::unique_ptr<Ground> ground_;

	std::unique_ptr<Player> player_;
	std::unique_ptr<Boss> boss_;

	std::vector<std::unique_ptr<Rock>> rocks_;
	
	std::list<std::unique_ptr<PlayerAttack>> playerAttacks_;
	std::unique_ptr<GPUParticle> attackEndEff_;
	std::unique_ptr<GPUParticle> deadEff_;

	bool isDeadEff_ = false;

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

	PostEffect* postEffect_ = nullptr;
	OutLine* outLine_ = nullptr;
	HSVFilter* hsvFilter_ = nullptr;

	std::unique_ptr<LevelData> levelData_;

	uint32_t burnScarsTex_ = 0;

	GroundFlare* groundFlare_ = nullptr;
	IcicleManager* icicle_ = nullptr;
	PlasmaShotManager* plasmaShot_ = nullptr;
	ElementBallManager* elementBall_ = nullptr;


	bool isGameStop_ = false;

	uint32_t skyBoxTex_ = 0;
	std::unique_ptr<SkyBox> skyBox_;

private:

	enum class SceneEvent {
		Battle,
		PlayerDead,
		BossDead,
		Clear,
		GameOver,
	};

private:

	void BattleInit();
	void BattleUpdate();

	void PlayerDeadInit();
	void PlayerDeadUpdate();

	void BossDeadInit();
	void BossDeadUpdate();

	void ClearInit();
	void ClearUpdate();

	void GameOverInit();
	void GameOverUpdate();

private:

	SceneEvent sceneEvent_ = SceneEvent::Battle;
	std::optional<SceneEvent> eventRequest_ = SceneEvent::Battle;

	std::map<SceneEvent, std::function<void()>> scenEventInitTable_{
		{SceneEvent::Battle,[this]() {BattleInit(); }},
		{SceneEvent::PlayerDead,[this]() {PlayerDeadInit(); }},
		{SceneEvent::BossDead,[this]() {BossDeadInit(); }},
		{SceneEvent::Clear,[this]() { ClearInit(); }},
		{SceneEvent::GameOver,[this]() {GameOverInit(); }},
	};

	std::map<SceneEvent, std::function<void()>> sceneEventUpdateTable_{
		{SceneEvent::Battle,[this]() {BattleUpdate(); }},
		{SceneEvent::PlayerDead,[this]() {PlayerDeadUpdate(); }},
		{SceneEvent::BossDead,[this]() {BossDeadUpdate(); }},
		{SceneEvent::Clear,[this]() {ClearUpdate(); }},
		{SceneEvent::GameOver,[this]() {GameOverUpdate(); }},
	};

private:

	struct WorkBossDead {
		int32_t interval_ = 30;
		int32_t count_ = 0;
		Vector3 cameraPos_ = {};
		Vector3 cameraRotate_ = { 0.2f,0.34f,0.0f };
		Vector3 offset_ = { -3.0f,2.0f,-8.0f };
	};

	struct WorkPlayerDead {
		int32_t interval_ = 30;
		int32_t count_ = 0;
		Vector3 cameraPos_ = {};
		Vector3 cameraRotate_ = { -0.14f,-2.9f,0.0f };
		Vector3 offset_ = { 1.5f, -0.5f, 6.0f };
	};

	WorkBossDead workBossDead_;
	WorkPlayerDead workPlayerDead_;

};

