#pragma once
///---------------------------------------------------------------------------------------------
// 
// ゲームシーン
// 
///---------------------------------------------------------------------------------------------


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
#include "PlayerMagicBall.h"
#include "BurnScar.h"
#include "IceScar.h"
#include "GroundFlare.h"
#include "IcicleManager.h"
#include "PlasmaShotManager.h"
#include "Rock.h"

//ゲームシーンクラス
class GameScene : public IScene {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Init()override;
	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;
	/// <summary>
	/// 背景描画
	/// </summary>
	void DrawBackGround()override;
	/// <summary>
	/// モデル描画
	/// </summary>
	void DrawModel()override;
	/// <summary>
	/// パーティクル3dモデル描画
	/// </summary>
	void DrawParticleModel()override;
	/// <summary>
	/// パーティクル描画
	/// </summary>
	void DrawParticle()override;
	/// <summary>
	/// UI描画
	/// </summary>
	void DrawUI()override;
	/// <summary>
	/// ポストエフェクトを掛けるモデルなどの描画
	/// </summary>
	void DrawPostEffect()override;
	/// <summary>
	/// レンダーテクスチャの描画
	/// </summary>
	void DrawRenderTexture()override;
	/// <summary>
	/// デバッグ用
	/// </summary>
	void DebugGUI()override;
	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene()override;
	/// <summary>
	/// コンストラクタ
	/// </summary>
	GameScene();
	/// <summary>
	/// プレイヤー攻撃をリストに追加
	/// </summary>
	/// <param name="playerAttack">プレイヤー攻撃クラスのポインタ</param>
	void AddPlayerAttack(PlayerMagicBall* playerAttack);

private:
	//カメラ
	Camera camera_;
	//ポイントライト
	PointLight pointLight_;
	//スポットライト
	SpotLight spotLight_;

	//地面
	std::unique_ptr<Ground> ground_;
	//プレイヤー
	std::unique_ptr<Player> player_;
	//ボス
	std::unique_ptr<Boss> boss_;
	//岩
	std::vector<std::unique_ptr<Rock>> rocks_;
	
	//プレイヤーの攻撃
	std::list<std::unique_ptr<PlayerMagicBall>> playerAttacks_;
	//プレイヤー攻撃が消えたときのエフェクト
	std::map<std::string, std::unique_ptr<GPUParticle>> attackEndEff_;
	//追従カメラ
	std::unique_ptr<FollowCamera> followCamera_;
	//戦闘が終了してからタイトルに戻るまでの時間
	uint32_t finishTime_ = 60 * 3;
	//ゲーム終了カウンター
	uint32_t finishCount_ = finishTime_;

	///UI
	//Xボタン
	std::unique_ptr<Sprite> XButton_;
	//攻撃文字
	std::unique_ptr<Sprite> char_Attack_;
	//ゲームオーバー文字
	std::unique_ptr<Sprite> gameOver_;
	//終了文字
	std::unique_ptr<Sprite> finish_;
	///

	//ゲームオーバー文字のフェード用α値
	float alpha_ = 0.0f;

	//ポストエフェクト
	PostEffect* postEffect_ = nullptr;
	//アウトライン
	OutLine* outLine_ = nullptr;
	//HSVフィルター
	HSVFilter* hsvFilter_ = nullptr;
	//レベルデータ
	std::unique_ptr<LevelData> levelData_;

	///敵の攻撃の各種マネージャ
	//地面から火が出るやつ
	std::unique_ptr<GroundFlare> groundFlare_;
	//つらら
	std::unique_ptr<IcicleManager> icicle_;
	//電気玉
	std::unique_ptr<PlasmaShotManager> plasmaShot_;
	//火の玉
	std::unique_ptr<ElementBallManager> elementBall_;
	///

	//ゲームを止めるか
	bool isGameStop_ = false;

	//スカイボックス
	std::unique_ptr<SkyBox> skyBox_;

private:
	//シーンイベント
	enum class SceneEvent {
		//バトル
		Battle,
		//プレイヤー死亡
		PlayerDead,
		//ボス死亡
		BossDead,
		//クリア
		Clear,
		//ゲームオーバー
		GameOver,
	};

private:
	/// <summary>
	/// バトル初期化
	/// </summary>
	void BattleInit();
	/// <summary>
	/// バトル更新
	/// </summary>
	void BattleUpdate();
	/// <summary>
	/// プレイヤー死亡初期化
	/// </summary>
	void PlayerDeadInit();
	/// <summary>
	/// プレイヤー死亡更新
	/// </summary>
	void PlayerDeadUpdate();
	/// <summary>
	/// ボス死亡初期化
	/// </summary>
	void BossDeadInit();
	/// <summary>
	/// ボス死亡更新
	/// </summary>
	void BossDeadUpdate();
	/// <summary>
	/// クリア初期化
	/// </summary>
	void ClearInit();
	/// <summary>
	/// クリア更新
	/// </summary>
	void ClearUpdate();
	/// <summary>
	/// ゲームオーバー初期化
	/// </summary>
	void GameOverInit();
	/// <summary>
	/// ゲームオーバー更新
	/// </summary>
	void GameOverUpdate();

private:
	//現在のシーンイベント
	SceneEvent sceneEvent_ = SceneEvent::Battle;
	//次のシーンイベントのリクエスト
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
	//ボス死亡イベントのパラメータ
	WorkBossDead workBossDead_;
	//プレイヤー死亡イベントのパラメータ
	WorkPlayerDead workPlayerDead_;

};

