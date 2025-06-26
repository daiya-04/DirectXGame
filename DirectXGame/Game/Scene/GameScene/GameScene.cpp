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
#include "MagicBallManager.h"
#include "GroundBurstManager.h"
#include "MagicCannonManager.h"

GameScene::GameScene() {
	
}

GameScene::~GameScene() {
	postEffect_->SetGrayScaleEffect(false);
}

void GameScene::SetGlobalVariables() {
	DaiEngine::GlobalVariables* globalVariables = DaiEngine::GlobalVariables::GetInstance();

	std::string groupName = "AttackButtonIcon";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Position", attackButtonIcon_->GetPosition());
	globalVariables->AddItem(groupName, "Scale", attackButtonIcon_->GetScale().x);

	groupName = "AvoidButtonIcon";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Position", avoidButtonIcon_->GetPosition());
	globalVariables->AddItem(groupName, "Scale", avoidButtonIcon_->GetScale().x);

	groupName = "DashButtonIcon";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Position", dashButtonIcon_->GetPosition());
	globalVariables->AddItem(groupName, "Scale", dashButtonIcon_->GetScale().x);

	groupName = "ChargeAttackButtonIcon";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Position", chargeAttackButtonIcon_->GetPosition());
	globalVariables->AddItem(groupName, "Scale", chargeAttackButtonIcon_->GetScale().x);

	groupName = "charAttack";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Position", charAttack_->GetPosition());
	globalVariables->AddItem(groupName, "Scale", charAttack_->GetScale().x);

	groupName = "charDash";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Position", charDash_->GetPosition());
	globalVariables->AddItem(groupName, "Scale", charDash_->GetScale().x);

	groupName = "charAvoid";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Position", charAvoid_->GetPosition());
	globalVariables->AddItem(groupName, "Scale", charAvoid_->GetScale().x);

	groupName = "charChargeAttack";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Position", charChargeAttack_->GetPosition());
	globalVariables->AddItem(groupName, "Scale", charChargeAttack_->GetScale().x);

	groupName = "charLongPush";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Position", charLongPush_->GetPosition());
	globalVariables->AddItem(groupName, "Scale", charLongPush_->GetScale().x);

	groupName = "gameOver";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Position", gameOver_->GetPosition());

	groupName = "finish";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Position", finish_->GetPosition());

}

void GameScene::ApplyGlobalVariables() {
	DaiEngine::GlobalVariables* globalVariables = DaiEngine::GlobalVariables::GetInstance();
	
	std::string groupName = "AttackButtonIcon";
	attackButtonIcon_->SetPosition(globalVariables->GetVec2Value(groupName, "Position"));
	attackButtonIcon_->SetScale(globalVariables->GetFloatValue(groupName, "Scale"));

	groupName = "AvoidButtonIcon";
	avoidButtonIcon_->SetPosition(globalVariables->GetVec2Value(groupName, "Position"));
	avoidButtonIcon_->SetScale(globalVariables->GetFloatValue(groupName, "Scale"));

	groupName = "DashButtonIcon";
	dashButtonIcon_->SetPosition(globalVariables->GetVec2Value(groupName, "Position"));
	dashButtonIcon_->SetScale(globalVariables->GetFloatValue(groupName, "Scale"));

	groupName = "ChargeAttackButtonIcon";
	chargeAttackButtonIcon_->SetPosition(globalVariables->GetVec2Value(groupName, "Position"));
	chargeAttackButtonIcon_->SetScale(globalVariables->GetFloatValue(groupName, "Scale"));

	groupName = "charAttack";
	charAttack_->SetPosition(globalVariables->GetVec2Value(groupName, "Position"));
	charAttack_->SetScale(globalVariables->GetFloatValue(groupName, "Scale"));

	groupName = "charDash";
	charDash_->SetPosition(globalVariables->GetVec2Value(groupName, "Position"));
	charDash_->SetScale(globalVariables->GetFloatValue(groupName, "Scale"));

	groupName = "charAvoid";
	charAvoid_->SetPosition(globalVariables->GetVec2Value(groupName, "Position"));
	charAvoid_->SetScale(globalVariables->GetFloatValue(groupName, "Scale"));

	groupName = "charChargeAttack";
	charChargeAttack_->SetPosition(globalVariables->GetVec2Value(groupName, "Position"));
	charChargeAttack_->SetScale(globalVariables->GetFloatValue(groupName, "Scale"));

	groupName = "charLongPush";
	charLongPush_->SetPosition(globalVariables->GetVec2Value(groupName, "Position"));
	charLongPush_->SetScale(globalVariables->GetFloatValue(groupName, "Scale"));

	groupName = "gameOver";
	gameOver_->SetPosition(globalVariables->GetVec2Value(groupName, "Position"));

	groupName = "finish";
	finish_->SetPosition(globalVariables->GetVec2Value(groupName, "Position"));

}

void GameScene::Init(){
	//カメラ初期化
	camera_.Init();
	camera_.SetFOV(60.0f);
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
	std::shared_ptr<DaiEngine::Model> playerDashModel = DaiEngine::ModelManager::LoadGLTF("PlayerDash");
	std::shared_ptr<DaiEngine::Model> playerKnockBackModel = DaiEngine::ModelManager::LoadGLTF("KnockBack");
	std::shared_ptr<DaiEngine::Model> playerAvoidModel = DaiEngine::ModelManager::LoadGLTF("PlayerAvoid");
	std::shared_ptr<DaiEngine::Model> playerChargeModel = DaiEngine::ModelManager::LoadGLTF("Charge");
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
	uint32_t BButtonTex = DaiEngine::TextureManager::Load("BButton.png");
	uint32_t char_AttackTex = DaiEngine::TextureManager::Load("char_Attack.png");
	uint32_t char_DashTex = DaiEngine::TextureManager::Load("char_Dash.png");
	uint32_t char_AvoidTex = DaiEngine::TextureManager::Load("char_Avoid.png");
	uint32_t char_ChargeAttack = DaiEngine::TextureManager::Load("char_ChargeAttack.png");
	uint32_t char_LongPush = DaiEngine::TextureManager::Load("char_longPush.png");
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
	player_->Init({ playerStandingModel, playerRunningModel, playerAttackModel, playerAttackC2Model, playerAttackC3Model, playerDeadModel, playerDashModel, playerKnockBackModel, playerAvoidModel, playerChargeModel });


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
	//氷爆発
	bossAttacks_.push_back(std::make_unique<IceImpactManager>());

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
	//チャージ攻撃
	playerAttacks_.push_back(std::make_unique<MagicCannonManager>());

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
	
	attackButtonIcon_.reset(DaiEngine::Sprite::Create(XButtonTex, {}));

	avoidButtonIcon_.reset(DaiEngine::Sprite::Create(AButtonTex, {}));

	dashButtonIcon_.reset(DaiEngine::Sprite::Create(BButtonTex, {}));

	chargeAttackButtonIcon_.reset(DaiEngine::Sprite::Create(XButtonTex, {}));

	charAttack_.reset(DaiEngine::Sprite::Create(char_AttackTex, {}));

	charDash_.reset(DaiEngine::Sprite::Create(char_DashTex, {}));

	charAvoid_.reset(DaiEngine::Sprite::Create(char_AvoidTex, {}));

	charChargeAttack_.reset(DaiEngine::Sprite::Create(char_ChargeAttack, {}));

	charLongPush_.reset(DaiEngine::Sprite::Create(char_LongPush, {}));

	gameOver_.reset(DaiEngine::Sprite::Create(gameOverTex, {}));

	finish_.reset(DaiEngine::Sprite::Create(finishTex, {}));

	///

	gameTime_ = std::make_unique<GameTime>();
	gameTime_->Init();

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
	GetBossAttackType<GroundFlareManager>()->DrawBurnScars(camera_);

	IceScar::preDraw();
	GetBossAttackType<IcicleManager>()->DrawScar(camera_);
	GetBossAttackType<IceImpactManager>()->DrawScar(camera_);

	skyBox_->Draw(camera_);

	boss_->DrawParticle(camera_);
	player_->DrawParticle(camera_);

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
