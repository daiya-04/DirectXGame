#pragma once
#include "IScene.h"
#include <memory>
#include <list>
#include <vector>
#include <optional>
#include <map>
#include <string>
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
#include "BurnScar.h"
#include "IceScar.h"
#include "GroundFlare.h"
#include "IcicleManager.h"
#include "PlasmaShotManager.h"
#include "Rock.h"


class GameScene : public IScene {
public:
	//初期化
	void Init()override;
	//更新
	void Update()override;
	//背景描画
	void DrawBackGround()override;
	//モデル描画
	void DrawModel()override;
	//パーティクル3dモデル描画
	void DrawParticleModel()override;
	//パーティクル描画
	void DrawParticle()override;
	//UI描画
	void DrawUI()override;
	//ポストエフェクトを掛けるモデルなどの描画
	void DrawPostEffect()override;
	//レンダーテクスチャの描画
	void DrawRenderTexture()override;
	//デバッグ用
	void DebugGUI()override;

	~GameScene()override;

	GameScene();
	//プレイヤー攻撃追加
	void AddPlayerAttack(PlayerAttack* playerAttack);

private:
  
	Camera camera_;

	PointLight pointLight_;
	SpotLight spotLight_;

private: //オブジェクト

	//地面
	std::unique_ptr<Ground> ground_;
	//プレイヤー
	std::unique_ptr<Player> player_;
	//ボス
	std::unique_ptr<Boss> boss_;
	//岩
	std::vector<std::unique_ptr<Rock>> rocks_;
	
	//プレイヤーの攻撃
	std::list<std::unique_ptr<PlayerAttack>> playerAttacks_;
	std::map<std::string, std::unique_ptr<GPUParticle>> attackEndEff_;
	//追従カメラ
	std::unique_ptr<FollowCamera> followCamera_;
	//戦闘が終了してからタイトルに戻るまでの時間
	uint32_t finishTime_ = 60 * 3;
	uint32_t finishCount_ = finishTime_;
	//UI
	std::unique_ptr<Sprite> XButton_;
	std::unique_ptr<Sprite> char_Attack_;
	std::unique_ptr<Sprite> gameOver_;
	std::unique_ptr<Sprite> finish_;

	float alpha_ = 0.0f;

	//ポストエフェクト
	PostEffect* postEffect_ = nullptr;
	OutLine* outLine_ = nullptr;
	HSVFilter* hsvFilter_ = nullptr;
	//レベルデータ
	std::unique_ptr<LevelData> levelData_;

	uint32_t burnScarsTex_ = 0;

	//敵の攻撃の各種マネージャ
	std::unique_ptr<GroundFlare> groundFlare_;
	std::unique_ptr<IcicleManager> icicle_;
	std::unique_ptr<PlasmaShotManager> plasmaShot_;
	std::unique_ptr<ElementBallManager> elementBall_;

	//ゲームを止めるか
	bool isGameStop_ = false;

	//スカイボックス
	uint32_t skyBoxTex_ = 0;
	std::unique_ptr<SkyBox> skyBox_;

private:
	//シーンイベント
	enum class SceneEvent {
		Battle,
		PlayerDead,
		BossDead,
		Clear,
		GameOver,
	};

private:
	//バトル
	void BattleInit();
	void BattleUpdate();
	//プレイヤー死亡
	void PlayerDeadInit();
	void PlayerDeadUpdate();
	//ボス死亡
	void BossDeadInit();
	void BossDeadUpdate();
	//クリア
	void ClearInit();
	void ClearUpdate();
	//ゲームオーバー
	void GameOverInit();
	void GameOverUpdate();

private:

	SceneEvent sceneEvent_ = SceneEvent::Battle;
	std::optional<SceneEvent> eventRequest_ = SceneEvent::Battle;
	//シーンイベント初期化テーブル
	std::map<SceneEvent, std::function<void()>> scenEventInitTable_{
		{SceneEvent::Battle,[this]() {BattleInit(); }},
		{SceneEvent::PlayerDead,[this]() {PlayerDeadInit(); }},
		{SceneEvent::BossDead,[this]() {BossDeadInit(); }},
		{SceneEvent::Clear,[this]() { ClearInit(); }},
		{SceneEvent::GameOver,[this]() {GameOverInit(); }},
	};
	//シーンイベント更新テーブル
	std::map<SceneEvent, std::function<void()>> sceneEventUpdateTable_{
		{SceneEvent::Battle,[this]() {BattleUpdate(); }},
		{SceneEvent::PlayerDead,[this]() {PlayerDeadUpdate(); }},
		{SceneEvent::BossDead,[this]() {BossDeadUpdate(); }},
		{SceneEvent::Clear,[this]() {ClearUpdate(); }},
		{SceneEvent::GameOver,[this]() {GameOverUpdate(); }},
	};

private:
	//ボス死亡シーンに必要なパラメータ
	struct WorkBossDead {
		//死亡アニメーションが終わってからの間の時間
		int32_t interval_ = 30;
		//死亡アニメーションが終わってからの時間
		int32_t count_ = 0;
		//カメラの位置
		Vector3 cameraPos_ = {};
		//カメラの回転角
		Vector3 cameraRotate_ = { 0.2f,0.34f,0.0f };
		//ターゲットからのオフセット
		Vector3 offset_ = { -3.0f,2.0f,-8.0f };
	};
	//プレイヤー死亡シーンに必要なパラメータ
	struct WorkPlayerDead {
		//死亡アニメーションが終わってからの間
		int32_t interval_ = 30;
		//死亡アニメーションが終わってからの時間
		int32_t count_ = 0;
		//カメラの位置
		Vector3 cameraPos_ = {};
		//カメラの回転角
		Vector3 cameraRotate_ = { -0.14f,-2.9f,0.0f };
		//ターゲットからのオフセット
		Vector3 offset_ = { 1.5f, -0.5f, 6.0f };
	};

	WorkBossDead workBossDead_;
	WorkPlayerDead workPlayerDead_;

};

