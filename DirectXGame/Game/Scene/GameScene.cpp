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
	Object3d::SetPointLight(&pointLight_);
	Object3d::SetSpotLight(&spotLight_);

	finishCount_ = finishTime_;

	//ステージ情報読み込み
	levelData_ = std::unique_ptr<LevelData>(LevelLoader::LoadFile("stage"));

	/// モデルの読み込み

	std::shared_ptr<Model> skydomeModel = ModelManager::LoadOBJ("skydome");
	std::shared_ptr<Model> groundModel = ModelManager::LoadOBJ("ground");
	//プレイヤー
	std::shared_ptr<Model> playerStandingModel = ModelManager::LoadGLTF("Standing");
	std::shared_ptr<Model> playerRunningModel = ModelManager::LoadGLTF("Running");
	std::shared_ptr<Model> playerAttackModel = ModelManager::LoadGLTF("PlayerAttack");
	std::shared_ptr<Model> playerDeadModel = ModelManager::LoadGLTF("PlayerDead");
	std::shared_ptr<Model> playerAccelModel = ModelManager::LoadGLTF("Accel");
	std::shared_ptr<Model> playerKnockBackModel = ModelManager::LoadGLTF("KnockBack");
	//ボス
	std::shared_ptr<Model> bossStandingModel = ModelManager::LoadGLTF("Standing");
	std::shared_ptr<Model> bossSetModel = ModelManager::LoadGLTF("SetMotion");
	std::shared_ptr<Model> bossAttackModel = ModelManager::LoadGLTF("BossAttack");
	std::shared_ptr<Model> bossDeadModel = ModelManager::LoadGLTF("BossDead");

	std::shared_ptr<Model> warningZoneModel = ModelManager::LoadOBJ("WarningZone");
	std::shared_ptr<Model> icicleModel = ModelManager::LoadOBJ("Icicle");
	std::shared_ptr<Model> plasmaBallModel = ModelManager::LoadOBJ("PlasmaBall");
	std::shared_ptr<Model> elementBallModel = ModelManager::LoadGLTF("ElementBall");
	std::shared_ptr<Model> rockModel = ModelManager::LoadOBJ("Rock");

	///

	///テクスチャの読み込み

	uint32_t finishTex = TextureManager::Load("finish.png");
	uint32_t XButtonTex = TextureManager::Load("XButton.png");
	uint32_t AButtonTex = TextureManager::Load("AButton_P.png");
	uint32_t char_AttackTex = TextureManager::Load("char_Attack.png");
	uint32_t char_DashTex = TextureManager::Load("char_Dash.png");
	uint32_t gameOverTex = TextureManager::Load("GameOver.png");
	uint32_t skyBoxTex = TextureManager::Load("skyBox.dds");

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
	skyBox_.reset(SkyBox::Create(skyBoxTex));

	//地面
	ground_ = std::make_unique<Ground>();
	ground_->Init(groundModel);

	//プレイヤー
	player_ = std::make_unique<Player>();
	player_->Init({ playerStandingModel,playerRunningModel,playerAttackModel,playerDeadModel,playerAccelModel,playerKnockBackModel });


	//ボス
	boss_ = std::make_unique<Boss>();
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
	groundFlare_ = std::make_unique<GroundFlare>();
	groundFlare_->Init(warningZoneModel);
	groundFlare_->SetTerget(&player_->GetCenterPos());
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

	//UI
	
	XButton_.reset(Sprite::Create(XButtonTex, {1200.0f,70.0f}));

	AButton_.reset(Sprite::Create(AButtonTex, { 1200.0f,170.0f }));

	char_Attack_.reset(Sprite::Create(char_AttackTex, {1080.0f,70.0f}));

	char_Dash_.reset(Sprite::Create(char_DashTex, { 1080.0f,170.0f }));

	gameOver_.reset(Sprite::Create(gameOverTex, { 670.0f,200.0f }));

	finish_.reset(Sprite::Create(finishTex, { 670.0f,200.0f }));

	///

}

void GameScene::Update() {
	DebugGUI();

#ifdef _DEBUG
	//デバッグ用シーンの切り替えコマンド
	if (Input::GetInstance()->PushKey(DIK_LCONTROL) && Input::GetInstance()->TriggerKey(DIK_1)) {
		SceneManager::GetInstance()->ChangeScene("Title");
	}
	if (Input::GetInstance()->PushKey(DIK_LCONTROL) && Input::GetInstance()->TriggerKey(DIK_3)) {
		SceneManager::GetInstance()->ChangeScene("Debug");
	}

#endif // _DEBUG

	if (isGameStop_) { return; }

	if (player_->IsDead() && sceneEvent_ == SceneEvent::Battle) {
		eventRequest_ = SceneEvent::PlayerDead;
	}

	if (boss_->IsDead() && sceneEvent_ == SceneEvent::Battle) {
		eventRequest_ = SceneEvent::BossDead;
	}

	if (eventRequest_) {

		sceneEvent_ = eventRequest_.value();
		//シーンイベント初期化
		scenEventInitTable_[sceneEvent_]();

		eventRequest_ = std::nullopt;
	}

	//シーンイベント更新
	sceneEventUpdateTable_[sceneEvent_]();

	

	for (auto& rock : rocks_) {
		rock->Update();
	}
	
	ground_->Update();

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

	if (sceneEvent_ == SceneEvent::Battle) {
		XButton_->Draw();
		AButton_->Draw();
		char_Attack_->Draw();
		char_Dash_->Draw();
		player_->DrawUI();
		boss_->DrawUI();
	}
	if (sceneEvent_ == SceneEvent::GameOver) {
		gameOver_->Draw();
	}
	if (sceneEvent_ == SceneEvent::Clear) {
		finish_->Draw();
	}


	
}

void GameScene::DrawPostEffect() {

	///

	outLine_->PreDrawScene(DirectXCommon::GetInstance()->GetCommandList());

	SkinningObject::preDraw();
	if (sceneEvent_ != SceneEvent::Clear) {
		boss_->Draw(camera_);
	}
	if (sceneEvent_ != SceneEvent::GameOver) {
		player_->Draw(camera_);
	}

	outLine_->PostDrawScene(DirectXCommon::GetInstance()->GetCommandList());

	///

	postEffect_->PreDrawScene(DirectXCommon::GetInstance()->GetCommandList());

	outLine_->Draw(DirectXCommon::GetInstance()->GetCommandList());

	Object3d::preDraw();
	
	ground_->Draw(camera_);
	
	if (sceneEvent_ == SceneEvent::Battle || sceneEvent_ == SceneEvent::Clear) {
		player_->DrawAttack(camera_);
	}
	
	if (sceneEvent_ == SceneEvent::Battle) {
		icicle_->Draw(camera_);
		plasmaShot_->Draw(camera_);

		groundFlare_->Draw(camera_);
		elementBall_->Draw(camera_);
	}
	

	for (auto& rock : rocks_) {
		rock->Draw(camera_);
	}
	
	BurnScar::preDraw();
	elementBall_->DrawBurnScars(camera_);

	IceScar::preDraw();
	icicle_->DrawScar(camera_);

	skyBox_->Draw(camera_);

	GPUParticle::preDraw();

	boss_->DrawParticle(camera_);

	if (sceneEvent_ == SceneEvent::Battle || sceneEvent_ == SceneEvent::Clear) {
		player_->DrawParticle(camera_);
	}
	
	
	
	if (sceneEvent_ == SceneEvent::Battle) {
		groundFlare_->DrawParticle(camera_);
		icicle_->DrawParticle(camera_);
		plasmaShot_->DrawParticle(camera_);
		elementBall_->DrawParticle(camera_);
	}
	

	postEffect_->PostDrawScene(DirectXCommon::GetInstance()->GetCommandList());

	///

	hsvFilter_->PreDrawScene();

	postEffect_->Draw(DirectXCommon::GetInstance()->GetCommandList());

	hsvFilter_->PostDrawScene();

	///

}

void GameScene::DrawRenderTexture() {
	hsvFilter_->Draw();
}

void GameScene::BattleInit() {



}

void GameScene::BattleUpdate() {
	
	player_->Update();
	boss_->Update();
	
	followCamera_->Update();

	groundFlare_->Update();
	icicle_->Update();
	plasmaShot_->Update();
	elementBall_->Update();

	///衝突判定

	////プレイヤーと属性球
	//if (elementBall_->IsAttack()) {
	//	for (uint32_t index = 0; index < elementBall_->GetElementballCount(); index++) {
	//		if (!elementBall_->IsLife(index)) { 
	//			continue;
	//		}
	//		if (IsCollision(player_->GetCollider(), elementBall_->GetCollider(index))) {
	//			player_->OnCollision(elementBall_->GetWorldPos(index));
	//			elementBall_->OnCollision(index);
	//		}
	//	}
	//}

	////プレイヤーと火が噴き出すやつ
	//if (groundFlare_->IsHit()) {
	//	if (IsCollision(groundFlare_->GetCollider(), player_->GetCollider())) {
	//		player_->OnCollision(groundFlare_->GetCenterPos());
	//		groundFlare_->OnCollision();
	//	}
	//}

	////プレイヤーとつらら
	//if (icicle_->IsAttack()) {
	//	for (uint32_t index = 0; index < icicle_->GetIcicleCount(); index++) {
	//		if (!icicle_->IsLife(index)) { continue; }
	//		if (IsCollision(player_->GetCollider(), icicle_->GetCollider(index))) {
	//			player_->OnCollision(icicle_->GetWorldPos(index));
	//			icicle_->OnCollision(index);
	//		}
	//	}
	//}

	////プレイヤーと電気玉
	//if (plasmaShot_->IsAttack()) {
	//	for (uint32_t index = 0; index < plasmaShot_->GetPlasmaShotCount(); index++) {
	//		if (!plasmaShot_->IsLife(index)) { continue; }
	//		if (IsCollision(player_->GetCollider(), plasmaShot_->GetCollider(index))) {
	//			player_->OnCollision(plasmaShot_->GetWorldPos(index));
	//			plasmaShot_->OnCollision(index);
	//		}
	//	}
	//}

	////ボスとプレイヤー攻撃
	//if (!playerAttacks_.empty()) {
	//	for (const auto& playerAttack : playerAttacks_) {
	//		if (IsCollision(boss_->GetCollider(), playerAttack->GetCollider())) {
	//			boss_->OnCollision();
	//			playerAttack->OnCollision();

	//			for (auto& [group, particle] : attackEndEff_) {
	//				particle->Emit();
	//				particle->particleData_.emitter_.translate = playerAttack->GetWorldPos();
	//			}
	//		}
	//	}
	//}

	ColliderManager::GetInstance()->CheckAllCollision();
	

	///

	camera_.SetMatView(followCamera_->GetCamera().GetMatView());

}

void GameScene::PlayerDeadInit() {

	workPlayerDead_.count_ = 0;

	workPlayerDead_.cameraPos_ = player_->GetCenterPos() + workPlayerDead_.offset_;
	camera_.translation_ = workPlayerDead_.cameraPos_;
	camera_.rotation_ = workPlayerDead_.cameraRotate_;

	camera_.UpdateViewMatrix();

}

void GameScene::PlayerDeadUpdate() {
	
	player_->Update();

	//死亡アニメーションが終わったらゲームオーバー演出
	if (player_->IsFinishDeadMotion()) {
		if (++workPlayerDead_.count_ >= workPlayerDead_.interval_) {
			eventRequest_ = SceneEvent::GameOver;
		}
	}
	
}

void GameScene::BossDeadInit() {

	workBossDead_.count_ = 0;

	workBossDead_.cameraPos_ = boss_->GetCenterPos() + workBossDead_.offset_;
	camera_.translation_ = workBossDead_.cameraPos_;
	camera_.rotation_ = workBossDead_.cameraRotate_;

	camera_.UpdateViewMatrix();

}

void GameScene::BossDeadUpdate() {

	boss_->Update();

	//死亡アニメーションが終わったらクリア
	if (boss_->IsFinishDeadMotion()) {
		if (++workBossDead_.count_ >= workBossDead_.interval_) {
			eventRequest_ = SceneEvent::Clear;
		}
	}

}

void GameScene::ClearInit() {
	alpha_ = 0.0f;
}

void GameScene::ClearUpdate() {
	//一定時間たったらタイトルへ
	if (--finishCount_ <= 0) {
		SceneManager::GetInstance()->ChangeScene("Title");
	}

	player_->Update();
	followCamera_->Update();

	camera_.SetMatView(followCamera_->GetCamera().GetMatView());

}

void GameScene::GameOverInit() {

	postEffect_->SetGrayScaleEffect(true);
	alpha_ = 0.0f;

}

void GameScene::GameOverUpdate() {
	
	alpha_ += 0.01f;
	alpha_ = std::min(alpha_, 1.0f);

	if (alpha_ >= 1.0f) {
		finishCount_--;
	}

	//一定時間たったらタイトルへ
	if (finishCount_ <= 0) {
		SceneManager::GetInstance()->ChangeScene("Title");
	}


	gameOver_->SetColor({ 1.0f,1.0f,1.0f,alpha_ });

}

void GameScene::DebugGUI(){
#ifdef _DEBUG
  
	ImGui::Begin("window");

	ImGui::Checkbox("GameStop", &isGameStop_);

	if (ImGui::Button("StageFileLoad")) {
		SceneManager::GetInstance()->ChangeScene("Game");
	}

	ImGui::End();

	hsvFilter_->DebugGUI();

#endif // _DEBUG
}
