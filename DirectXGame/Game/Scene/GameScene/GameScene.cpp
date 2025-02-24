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

GameScene::GameScene() {
	
}

GameScene::~GameScene() {
	postEffect_->SetGrayScaleEffect(false);
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

	std::shared_ptr<DaiEngine::Model> warningZoneModel = DaiEngine::ModelManager::LoadOBJ("WarningZone");
	std::shared_ptr<DaiEngine::Model> icicleModel = DaiEngine::ModelManager::LoadOBJ("Icicle");
	std::shared_ptr<DaiEngine::Model> plasmaBallModel = DaiEngine::ModelManager::LoadOBJ("PlasmaBall");
	std::shared_ptr<DaiEngine::Model> elementBallModel = DaiEngine::ModelManager::LoadGLTF("ElementBall");
	std::shared_ptr<DaiEngine::Model> rockModel = DaiEngine::ModelManager::LoadOBJ("Rock");

	///

	///テクスチャの読み込み

	uint32_t finishTex = DaiEngine::TextureManager::Load("finish.png");
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


	charactors_[static_cast<size_t>(CharactorType::kPlayer)] = std::make_unique<Player>();
	charactors_[static_cast<size_t>(CharactorType::kBoss)] = std::make_unique<Boss>();

	//プレイヤー
	player_ = dynamic_cast<Player*>(charactors_[static_cast<size_t>(CharactorType::kPlayer)].get());
	player_->Init({ playerStandingModel, playerRunningModel, playerAttackModel, playerAttackC2Model, playerAttackC3Model, playerDeadModel, playerAccelModel, playerKnockBackModel });


	//ボス
	boss_ = dynamic_cast<Boss*>(charactors_[static_cast<size_t>(CharactorType::kBoss)].get());
	boss_->Init({ bossStandingModel,bossSetModel,bossAttackModel,bossDeadModel });
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
	elementBall_ = std::make_unique<ElementBallManager>();
	elementBall_->Init(elementBallModel);
	elementBall_->SetTartget(&player_->GetCenterPos());
	boss_->SetElementBall(elementBall_.get());

	//地面から火が出るやつ
	groundFlare_ = std::make_unique<GroundFlareManager>();
	groundFlare_->Init(warningZoneModel);
	groundFlare_->SetTarget(&player_->GetCenterPos());
	boss_->SetGroudFlare(groundFlare_.get());

	//つらら飛ばすやつ
	icicle_ = std::make_unique<IcicleManager>();
	icicle_->Init(icicleModel);
	icicle_->SetTarget(&player_->GetCenterPos());
	boss_->SetIcicle(icicle_.get());

	//電気玉
	plasmaShot_ = std::make_unique<PlasmaShotManager>();
	plasmaShot_->Init(plasmaBallModel);
	plasmaShot_->SetTarget(&player_->GetCenterPos());
	boss_->SetPlasmaShot(plasmaShot_.get());

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
	
	XButton_.reset(DaiEngine::Sprite::Create(XButtonTex, {1200.0f,70.0f}));

	AButton_.reset(DaiEngine::Sprite::Create(AButtonTex, { 1200.0f,170.0f }));

	charAttack_.reset(DaiEngine::Sprite::Create(char_AttackTex, {1080.0f,70.0f}));

	charDash_.reset(DaiEngine::Sprite::Create(char_DashTex, { 1080.0f,170.0f }));

	gameOver_.reset(DaiEngine::Sprite::Create(gameOverTex, { 670.0f,200.0f }));

	finish_.reset(DaiEngine::Sprite::Create(finishTex, { 670.0f,200.0f }));

	///

	ChangeState(SceneEvent::Battle);

}

void GameScene::Update() {
	DebugGUI();

#ifdef _DEBUG
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

	DaiEngine::SkinningObject::preDraw();
	if (nowSceneEvent_ != SceneEvent::Clear) {
		boss_->Draw(camera_);
	}
	if (nowSceneEvent_ != SceneEvent::GameOver) {
		player_->Draw(camera_);
	}

	outLine_->PostDrawScene(DaiEngine::DirectXCommon::GetInstance()->GetCommandList());

	///

	postEffect_->PreDrawScene(DaiEngine::DirectXCommon::GetInstance()->GetCommandList());

	outLine_->Draw(DaiEngine::DirectXCommon::GetInstance()->GetCommandList());

	DaiEngine::Object3d::preDraw();
	
	ground_->Draw(camera_);
	
	if (nowSceneEvent_ == SceneEvent::Battle || nowSceneEvent_ == SceneEvent::Clear) {
		player_->DrawAttack(camera_);
	}
	
	if (nowSceneEvent_ == SceneEvent::Battle) {
		icicle_->Draw(camera_);
		plasmaShot_->Draw(camera_);

		groundFlare_->Draw(camera_);
	}
	

	for (auto& rock : rocks_) {
		rock->Draw(camera_);
	}
	
	BurnScar::preDraw();
	elementBall_->DrawBurnScars(camera_);

	IceScar::preDraw();
	icicle_->DrawScar(camera_);

	skyBox_->Draw(camera_);

	DaiEngine::GPUParticle::preDraw();

	boss_->DrawParticle(camera_);

	if (nowSceneEvent_ == SceneEvent::Battle || nowSceneEvent_ == SceneEvent::Clear) {
		player_->DrawParticle(camera_);
	}
	
	
	
	if (nowSceneEvent_ == SceneEvent::Battle) {
		groundFlare_->DrawParticle(camera_);
		icicle_->DrawParticle(camera_);
		plasmaShot_->DrawParticle(camera_);
		elementBall_->DrawParticle(camera_);
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

	const std::map<SceneEvent, std::function<std::unique_ptr<ISceneState>()>> stateTable{
		{SceneEvent::Battle, [this]() {return std::make_unique<BattleState>(this); }},
		{SceneEvent::PlayerDead, [this]() {return std::make_unique<PlayerDeadState>(this); }},
		{SceneEvent::BossDead, [this]() {return std::make_unique<BossDeadState>(this); }},
		{SceneEvent::Clear, [this]() {return std::make_unique<ClearState>(this); }},
		{SceneEvent::GameOver, [this]() {return std::make_unique<GameOverState>(this); }},
	};

	auto nextState = stateTable.find(stateName);
	if (nextState != stateTable.end()) {
		nowSceneEvent_ = nextState->first;
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
