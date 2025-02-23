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
#include "LockOn.h"
#include "ElementBallManager.h"
#include "PlayerMagicBall.h"
#include "BurnScar.h"
#include "IceScar.h"
#include "GroundFlare.h"
#include "IcicleManager.h"
#include "PlasmaShotManager.h"
#include "Rock.h"
#include "ISceneState.h"

//ゲームシーンクラス
class GameScene : public DaiEngine::IScene {
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

public:

	enum class CharactorType {
		kPlayer,
		kBoss,
	};


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

	SceneEvent nowSceneEvent_ = SceneEvent::Battle;

private:
	//カメラ
	DaiEngine::Camera camera_;
	//ポイントライト
	DaiEngine::PointLight pointLight_;
	//スポットライト
	DaiEngine::SpotLight spotLight_;
	//地面
	std::unique_ptr<Ground> ground_;

	std::array<std::unique_ptr<BaseCharactor>, 2> charactors_;
	//プレイヤー
	Player* player_ = nullptr;
	//ボス
	Boss* boss_ = nullptr;
	//岩
	std::vector<std::unique_ptr<Rock>> rocks_;
	//追従カメラ
	std::unique_ptr<FollowCamera> followCamera_;
	//
	std::unique_ptr<LockOn> lockOn_;
	//戦闘が終了してからタイトルに戻るまでの時間
	uint32_t finishTime_ = 60 * 3;
	//ゲーム終了カウンター
	uint32_t finishCount_ = finishTime_;

	///UI
	//Xボタン
	std::unique_ptr<DaiEngine::Sprite> XButton_;
	//Aボタン
	std::unique_ptr<DaiEngine::Sprite> AButton_;
	//攻撃文字
	std::unique_ptr<DaiEngine::Sprite> charAttack_;
	//走る文字
	std::unique_ptr<DaiEngine::Sprite> charDash_;
	//ゲームオーバー文字
	std::unique_ptr<DaiEngine::Sprite> gameOver_;
	//終了文字
	std::unique_ptr<DaiEngine::Sprite> finish_;
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
	std::unique_ptr<DaiEngine::SkyBox> skyBox_;

	std::unique_ptr<ISceneState> state_;

public:

	void ChangeState(SceneEvent stateName);

	DaiEngine::Camera& GetCamera() { return camera_; }

	//キャラクター取得
	std::array<std::unique_ptr<BaseCharactor>, 2>& GetCharactors() { return charactors_; }
	Player* GetPlayer() { return dynamic_cast<Player*>(charactors_[static_cast<size_t>(CharactorType::kPlayer)].get()); }
	Boss* GetBoss() { return dynamic_cast<Boss*>(charactors_[static_cast<size_t>(CharactorType::kBoss)].get()); }
	LockOn* GetLockOn() { return lockOn_.get(); }

	///攻撃各種取得
	GroundFlare* GetGroundFlare() { return groundFlare_.get(); }
	ElementBallManager* GetElementBall() { return elementBall_.get(); }
	PlasmaShotManager* GetPlasmaShot() { return plasmaShot_.get(); }
	IcicleManager* GetIcicle() { return icicle_.get(); }

	//追従カメラ取得
	FollowCamera* GetFollowCamera() { return followCamera_.get(); }

	///UI各種取得
	DaiEngine::Sprite* GetXButtonUI() { return XButton_.get(); }
	DaiEngine::Sprite* GetAButtonUI() { return AButton_.get(); }
	DaiEngine::Sprite* GetCharAttackUI() { return charAttack_.get(); }
	DaiEngine::Sprite* GetCharDashUI() { return charDash_.get(); }
	DaiEngine::Sprite* GetFinishUI() { return finish_.get(); }
	DaiEngine::Sprite* GetGameOverUI() { return gameOver_.get(); }

};

