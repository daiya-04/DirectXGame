#include "GameScene.h"

#include "DirectXCommon.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ParticleManager.h"
#include "ImGuiManager.h"
#include "Audio.h"
#include "Input.h"
#include "SceneManager.h"
#include "Hit.h"
#include <random>
#include <algorithm>
#include "ColliderManager.h"
#include "BattleState.h"
#include "PlayerDeadState.h"
#include "BossDeadState.h"
#include "ClearState.h"
#include "GameOverState.h"
#include "GlobalVariables.h"

GameScene::GameScene() {
	
}

GameScene::~GameScene() {
	postEffect_->SetGrayScaleEffect(false);
}

void GameScene::SetGlobalVariables() {
	DaiEngine::GlobalVariables* globalVariables = DaiEngine::GlobalVariables::GetInstance();

	std::string groupName = "XButton";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Position", XButton_->GetPosition());

	groupName = "AButton";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Position", AButton_->GetPosition());

	groupName = "charAttack";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Position", charAttack_->GetPosition());

	groupName = "charDash";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Position", charDash_->GetPosition());

	groupName = "gameOver";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Position", gameOver_->GetPosition());

	groupName = "finish";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Position", finish_->GetPosition());

}

void GameScene::ApplyGlobalVariables() {
	DaiEngine::GlobalVariables* globalVariables = DaiEngine::GlobalVariables::GetInstance();
	
	std::string groupName = "XButton";
	XButton_->SetPosition(globalVariables->GetVec2Value(groupName, "Position"));
	groupName = "AButton";
	AButton_->SetPosition(globalVariables->GetVec2Value(groupName, "Position"));
	groupName = "charAttack";
	charAttack_->SetPosition(globalVariables->GetVec2Value(groupName, "Position"));
	groupName = "charDash";
	charDash_->SetPosition(globalVariables->GetVec2Value(groupName, "Position"));
	groupName = "gameOver";
	gameOver_->SetPosition(globalVariables->GetVec2Value(groupName, "Position"));
	groupName = "finish";
	finish_->SetPosition(globalVariables->GetVec2Value(groupName, "Position"));

}

void GameScene::Init(){
	//カメラ初期化
	camera_.Init();
	//ライト初期化
	pointLight_.Init();
	spotLight_.Init();
	//object3dクラスにライトセット
	DaiEngine::Object3d::SetPointLight(&pointLight_);
	DaiEngine::Object3d::SetSpotLight(&spotLight_);

	finishCount_ = finishTime_;

	//ステージ情報読み込み
	levelData_ = std::unique_ptr<LevelData>(LevelLoader::LoadFile("stage"));

	/// モデルの読み込み

	std::shared_ptr<DaiEngine::Model> skydomeModel = DaiEngine::ModelManager::LoadOBJ("skydome");
	std::shared_ptr<DaiEngine::Model> groundModel = DaiEngine::ModelManager::LoadOBJ("ground");
	//プレイヤー
	std::shared_ptr<DaiEngine::Model> playerStandingModel = DaiEngine::ModelManager::LoadGLTF("Standing");
	std::shared_ptr<DaiEngine::Model> playerRunningModel = DaiEngine::ModelManager::LoadGLTF("Running");
	std::shared_ptr<DaiEngine::Model> playerAttackModel = DaiEngine::ModelManager::LoadGLTF("PlayerAttack");
	std::shared_ptr<DaiEngine::Model> playerAttackC2Model = DaiEngine::ModelManager::LoadGLTF("PlayerAttackC2");
	std::shared_ptr<DaiEngine::Model> playerAttackC3Model = DaiEngine::ModelManager::LoadGLTF("PlayerAttackC3");
	std::shared_ptr<DaiEngine::Model> playerDeadModel = DaiEngine::ModelManager::LoadGLTF("PlayerDead");
	std::shared_ptr<DaiEngine::Model> playerAccelModel = DaiEngine::ModelManager::LoadGLTF("Accel");
	std::shared_ptr<DaiEngine::Model> playerKnockBackModel = DaiEngine::ModelManager::LoadGLTF("KnockBack");
	//ボス
	std::shared_ptr<DaiEngine::Model> bossStandingModel = DaiEngine::ModelManager::LoadGLTF("Standing");
	std::shared_ptr<DaiEngine::Model> bossSetModel = DaiEngine::ModelManager::LoadGLTF("SetMotion");
	std::shared_ptr<DaiEngine::Model> bossAttackModel = DaiEngine::ModelManager::LoadGLTF("BossAttack");
	std::shared_ptr<DaiEngine::Model> bossDeadModel = DaiEngine::ModelManager::LoadGLTF("BossDead");

	std::shared_ptr<DaiEngine::Model> bossIdleModel = DaiEngine::ModelManager::LoadGLTF("Boss");

	std::shared_ptr<DaiEngine::Model> rockModel = DaiEngine::ModelManager::LoadOBJ("Rock");

	///

	///テクスチャの読み込み

	uint32_t finishTex = DaiEngine::TextureManager::Load("Clear_text.png");
	uint32_t XButtonTex = DaiEngine::TextureManager::Load("XButton.png");
	uint32_t AButtonTex = DaiEngine::TextureManager::Load("AButton_P.png");
	uint32_t char_AttackTex = DaiEngine::TextureManager::Load("char_Attack.png");
	uint32_t char_DashTex = DaiEngine::TextureManager::Load("char_Dash.png");
	uint32_t gameOverTex = DaiEngine::TextureManager::Load("GameOver.png");
	uint32_t skyBoxTex = DaiEngine::TextureManager::Load("skyBox.dds");

	postEffect_ = PostEffect::GetInstance();
	postEffect_->Init();
	postEffect_->SetGrayScaleEffect(false);

	outLine_ = OutLine::GetInstance();
	outLine_->Init();

	hsvFilter_ = HSVFilter::GetInstance();
	hsvFilter_->Init();

	///
	///オブジェクト初期化
	
	//天球
	skyBox_.reset(DaiEngine::SkyBox::Create(skyBoxTex));

	//地面
	ground_ = std::make_unique<Ground>();
	ground_->Init(groundModel);


	charactors_.push_back(std::make_unique<Player>());
	charactors_.push_back(std::make_unique<Boss>());

	//プレイヤー
	player_ = static_cast<Player*>(charactors_[static_cast<size_t>(CharactorType::kPlayer)].get());
	assert(player_);
	player_->Init({ playerStandingModel, playerRunningModel, playerAttackModel, playerAttackC2Model, playerAttackC3Model, playerDeadModel, playerAccelModel, playerKnockBackModel });


	//ボス
	boss_ = static_cast<Boss*>(charactors_[static_cast<size_t>(CharactorType::kBoss)].get());
	assert(boss_);
	boss_->Init({ bossIdleModel });
	boss_->SetTarget(&player_->GetWorldTransform());
	player_->SetTerget(&boss_->GetWorldTransform());

	///データのセット
	for (auto& objectData : levelData_->objectDatas_) {
		if (objectData.objectName == "Ground") {
			ground_->SetData(objectData);
		}
		if (objectData.objectName == "Player") {
			player_->SetData(objectData);
		}
		if (objectData.objectName == "Boss") {
			boss_->SetData(objectData);
		}
		if (objectData.objectName == "Rock") {
			auto& rock = rocks_.emplace_back(std::make_unique<Rock>());
			rock->Init(rockModel);
			rock->SetData(objectData);
		}
	}
	///

	///ボスの攻撃
	//属性球
	bossAttacks_.push_back(std::make_unique<ElementBallManager>());
	//地面から火が出るやつ
	bossAttacks_.push_back(std::make_unique<GroundFlareManager>());
	//つらら飛ばすやつ
	bossAttacks_.push_back(std::make_unique<IcicleManager>());
	//電気玉
	bossAttacks_.push_back(std::make_unique<PlasmaShotManager>());

	for (auto& bossAttack : bossAttacks_) {
		bossAttack->Init();
		bossAttack->SetTarget(&player_->GetCenterPos());
		boss_->SetAttack(bossAttack.get());
	}
	///

	///プレイヤーの攻撃
	//魔法弾
	playerAttacks_.push_back(std::make_unique<MagicBallManager>());
	//地面噴射
	playerAttacks_.push_back(std::make_unique<GroundBurstManager>());

	for (auto& playerAttack : playerAttacks_) {
		playerAttack->Init();
		player_->SetAttack(playerAttack.get());
	}
	///

	//追従カメラ
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Init();
	followCamera_->SetTarget(&player_->GetWorldTransform());
	player_->SetFollowCamera(followCamera_.get());
	camera_.SetMatView(followCamera_->GetCamera().GetMatView());
	camera_.UpdateCameraPos();

	//ロックオン
	lockOn_ = std::make_unique<LockOn>();
	lockOn_->Init();
	followCamera_->SetLockOn(lockOn_.get());

	//UI
	
	XButton_.reset(DaiEngine::Sprite::Create(XButtonTex, {}));

	AButton_.reset(DaiEngine::Sprite::Create(AButtonTex, {}));

	charAttack_.reset(DaiEngine::Sprite::Create(char_AttackTex, {}));

	charDash_.reset(DaiEngine::Sprite::Create(char_DashTex, {}));

	gameOver_.reset(DaiEngine::Sprite::Create(gameOverTex, {}));

	finish_.reset(DaiEngine::Sprite::Create(finishTex, {}));

	///

	//調整項目の追加と代入
	SetGlobalVariables();
	ApplyGlobalVariables();

	ChangeState(SceneEvent::Battle);

}

void GameScene::Update() {
	DebugGUI();

#ifdef _DEBUG
	//調整項目代入
	ApplyGlobalVariables();
	//デバッグ用シーンの切り替えコマンド
	if (DaiEngine::Input::GetInstance()->PushKey(DIK_LCONTROL) && DaiEngine::Input::GetInstance()->TriggerKey(DIK_1)) {
		DaiEngine::SceneManager::GetInstance()->ChangeScene("Title");
	}
	if (DaiEngine::Input::GetInstance()->PushKey(DIK_LCONTROL) && DaiEngine::Input::GetInstance()->TriggerKey(DIK_3)) {
		DaiEngine::SceneManager::GetInstance()->ChangeScene("Debug");
	}

#endif // _DEBUG

	if (isGameStop_) { return; }

	for (auto& rock : rocks_) {
		rock->Update();
	}
	ground_->Update();

	//シーンイベント更新
	state_->Update();

	//ライト更新
	pointLight_.Update();
	spotLight_.Update();

	//カメラ更新
	camera_.UpdateCameraPos();
}

void GameScene::DrawBackGround(){

	

}

void GameScene::DrawModel(){

	

}

void GameScene::DrawParticleModel(){



}

void GameScene::DrawParticle(){

}

void GameScene::DrawUI(){
	state_->DrawUI();
}

void GameScene::DrawPostEffect() {

	///

	outLine_->PreDrawScene(DaiEngine::DirectXCommon::GetInstance()->GetCommandList());

	for (auto& charactor : charactors_) {
		charactor->Draw(camera_);
	}

	outLine_->PostDrawScene(DaiEngine::DirectXCommon::GetInstance()->GetCommandList());

	///

	postEffect_->PreDrawScene(DaiEngine::DirectXCommon::GetInstance()->GetCommandList());

	outLine_->Draw(DaiEngine::DirectXCommon::GetInstance()->GetCommandList());

	
	ground_->Draw(camera_);
	
	if (nowSceneEvent_ == SceneEvent::Battle) {
		for (auto& bossAttack : bossAttacks_) {
			bossAttack->Draw(camera_);
		}
	}
	

	for (auto& rock : rocks_) {
		rock->Draw(camera_);
	}
	
	BurnScar::preDraw();
	GetBossAttackType<ElementBallManager>()->DrawBurnScars(camera_);

	IceScar::preDraw();
	GetBossAttackType<IcicleManager>()->DrawScar(camera_);

	skyBox_->Draw(camera_);

	boss_->DrawParticle(camera_);

	if (nowSceneEvent_ == SceneEvent::Battle || nowSceneEvent_ == SceneEvent::Clear) {
		for (auto& playerAttack : playerAttacks_) {
			playerAttack->DrawParticle(camera_);
		}
	}
	
	
	if (nowSceneEvent_ == SceneEvent::Battle) {
		for (auto& bossAttack : bossAttacks_) {
			bossAttack->DrawParticle(camera_);
		}
	}
	

	postEffect_->PostDrawScene(DaiEngine::DirectXCommon::GetInstance()->GetCommandList());

	///

	hsvFilter_->PreDrawScene();

	postEffect_->Draw(DaiEngine::DirectXCommon::GetInstance()->GetCommandList());

	hsvFilter_->PostDrawScene();

	///

}

void GameScene::DrawRenderTexture() {
	hsvFilter_->Draw();
}

void GameScene::ChangeState(SceneEvent stateName) {

	//シーンイベントとそれに対応するStateクラスの生成処理のマップ
	const std::map<SceneEvent, std::function<std::unique_ptr<ISceneState>()>> stateTable{
		{SceneEvent::Battle, [this]() {return std::make_unique<BattleState>(this); }},
		{SceneEvent::PlayerDead, [this]() {return std::make_unique<PlayerDeadState>(this); }},
		{SceneEvent::BossDead, [this]() {return std::make_unique<BossDeadState>(this); }},
		{SceneEvent::Clear, [this]() {return std::make_unique<ClearState>(this); }},
		{SceneEvent::GameOver, [this]() {return std::make_unique<GameOverState>(this); }},
	};
	//検索
	auto nextState = stateTable.find(stateName);
	if (nextState != stateTable.end()) {
		//現在のシーンイベント更新
		nowSceneEvent_ = nextState->first;
		//対応するStateクラスの生成と初期化
		state_ = nextState->second();
		state_->Init();
	}

}

void GameScene::DebugGUI(){
#ifdef _DEBUG
  
	ImGui::Begin("window");

	ImGui::Checkbox("GameStop", &isGameStop_);

	if (ImGui::Button("StageFileLoad")) {
		DaiEngine::SceneManager::GetInstance()->ChangeScene("Game");
	}

	ImGui::End();

	hsvFilter_->DebugGUI();

#endif // _DEBUG
}
