#include "GameScene.h"

#include "DirectXCommon.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include "Audio.h"
#include "Input.h"
#include "SceneManager.h"
#include "Hit.h"
#include <random>
#include <algorithm>

GameScene::GameScene() :randomEngine(seedGenerator()) {
	
}

GameScene::~GameScene() {
	postEffect_->SetGrayScaleEffect(false);
}

void GameScene::Init(){
  
	camera_.Init();
	pointLight_.Init();
	pointLight_.intensity_ = 0.0f;
	spotLight_.Init();
	spotLight_.intensity_ = 0.0f;

	Object3d::SetPointLight(&pointLight_);
	Object3d::SetSpotLight(&spotLight_);

	finishCount_ = finishTime_;

	levelData_ = std::unique_ptr<LevelData>(LevelLoader::LoadFile("stage"));

	/// モデルの読み込み

	std::shared_ptr<Model> skydomeModel = ModelManager::LoadOBJ("skydome");
	std::shared_ptr<Model> groundModel = ModelManager::LoadOBJ("ground");
	std::shared_ptr<Model> playerStandingModel = ModelManager::LoadGLTF("Standing");
	std::shared_ptr<Model> playerWalkingModel = ModelManager::LoadGLTF("Walking");
	std::shared_ptr<Model> playerAttackModel = ModelManager::LoadGLTF("PlayerAttack");
	//enemyBodyModel_ = ModelManager::Load("EnemyBody");
	//enemyHeadModel_ = ModelManager::Load("EnemyHead");
	//bulletModel_ = ModelManager::Load("EnemyBullet");
	std::shared_ptr<Model> bossStandingModel = ModelManager::LoadGLTF("Standing");
	std::shared_ptr<Model> bossSetModel = ModelManager::LoadGLTF("SetMotion");
	std::shared_ptr<Model> bossAttackModel = ModelManager::LoadGLTF("BossAttack");

	///

	///テクスチャの読み込み

	uint32_t particleTex = TextureManager::Load("circle.png");
	uint32_t XButtonTex = TextureManager::Load("XButton.png");
	uint32_t char_AttackTex = TextureManager::Load("char_Attack.png");
	uint32_t gameOverTex = TextureManager::Load("GameOver.png");
	burnScarsTex_ = TextureManager::Load("BurnScars.png");

	postEffect_ = PostEffect::GetInstance();
	postEffect_->Init();
	postEffect_->SetGrayScaleEffect(false);

	outLine_ = OutLine::GetInstance();
	outLine_->Init();

	///
	///オブジェクト初期化
	
	//天球
	skydome_ = std::make_unique<Skydome>();
	skydome_->Init(skydomeModel);

	//地面
	ground_ = std::make_unique<Ground>();
	ground_->Init(groundModel);

	//プレイヤー
	player_ = std::make_unique<Player>();
	player_->Init({ playerStandingModel,playerWalkingModel,playerAttackModel });

	//ボス
	boss_ = std::make_unique<Boss>();
	boss_->Init({ bossStandingModel,bossSetModel,bossAttackModel });
	boss_->SetGameScene(this);

	//敵
	/*uint32_t enemyNum = 3;

	std::vector<Vector3> enemyPos(enemyNum);*/

	/*enemyPos = {
		{20.0f,0.0f,10.0f},
		{-15.0f,0.0f,15.0f},
		{-5.0f,0.0f,-20.0f}
	};

	for (size_t index = 0; index < enemyNum; index++) {
		enemies_.push_back(std::unique_ptr<Enemy>(EnemyPop({ enemyBodyModel_ ,enemyHeadModel_ }, enemyPos[index])));
	}*/

	/*for (size_t index = 0; index < spawnNum_; index++) {
		std::uniform_int_distribution<int> distPosX(-60, 60);
		std::uniform_int_distribution<int> distPosZ(50, 100);
		Vector3 spawnPos = { (float)distPosX(randomEngine),0.0f,(float)distPosZ(randomEngine) };
		enemies_.push_back(std::unique_ptr<Enemy>(EnemyPop({ enemyBodyModel_ ,enemyHeadModel_ }, spawnPos)));
	}

	Enemy::SetTarget(&player_->GetWorldTransform());*/

	ElementBall::SetTarget(&player_->GetWorldTransform());

	//追従カメラ
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Init();
	followCamera_->SetTarget(&player_->GetWorldTransform());
	player_->SetFollowCamera(followCamera_.get());

	//パーティクル
	particle_ = std::make_unique<Particle>();
	particle_.reset(Particle::Create(particleTex, 100));

	//UI
	
	XButton_.reset(Sprite::Create(XButtonTex, {1200.0f,70.0f}));

	char_Attack_.reset(Sprite::Create(char_AttackTex, {1100.0f,70.0f}));

	gameOver_.reset(Sprite::Create(gameOverTex, { 670.0f,200.0f }));

	///

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
	}

	Update();

}

void GameScene::Update(){
	DebugGUI();

#ifdef _DEBUG

	if (Input::GetInstance()->PushKey(DIK_LCONTROL) && Input::GetInstance()->TriggerKey(DIK_1)) {
		SceneManager::GetInstance()->ChangeScene("Title");
	}
	if (Input::GetInstance()->PushKey(DIK_LCONTROL) && Input::GetInstance()->TriggerKey(DIK_3)) {
		SceneManager::GetInstance()->ChangeScene("Debug");
	}

#endif // _DEBUG

	if (eventRequest_) {

		sceneEvent_ = eventRequest_.value();

		switch (sceneEvent_) {
			case SceneEvent::Battle:
				BattleInit();
				break;
			case SceneEvent::PlayerDead:
				PlayerDeadInit();
				break;
		}

		eventRequest_ = std::nullopt;
	}

	/*if (gameCount_ >= 20) {
		if (--spawnCount_ <= 0) {
			spawnCount_ = spawnCoolTime_;
			for (size_t index = 0; index < spawnNum_; index++) {
				std::uniform_int_distribution<int> distPos(-60, 60);
				Vector3 spawnPos = { (float)distPos(randomEngine),0.0f,(float)distPos(randomEngine) };
				enemies_.push_back(std::unique_ptr<Enemy>(EnemyPop({ enemyBodyModel_ ,enemyHeadModel_ }, spawnPos)));
			}
		}
	}*/
	
	elementBalls_.remove_if([](const std::unique_ptr<ElementBall>& elementBall) {
		if (elementBall->IsLife()) {
			return true;
		}
		return false;
	});

	burnScarses_.remove_if([](const std::unique_ptr<BurnScars>& burnScars) {
		if (!burnScars->IsLife()) {
			return true;
		}
		return false;
	});

	switch (sceneEvent_) {
		case SceneEvent::Battle:
			BattleUpdate();
			break;
		case SceneEvent::PlayerDead:
			PlayerDeadUpdate();
			break;
	}

	

	skydome_->Update();
	ground_->Update();
	/*for (const auto& enemy : enemies_) {
		enemy->Update();
	}*/
	
	/*for (const auto& bullet : enemyBullets_) {
		bullet->Update();
	}*/

	for (std::list<Particle::ParticleData>::iterator itParticle = particles_.begin(); itParticle != particles_.end(); itParticle++) {
		(*itParticle).worldTransform_.translation_ += (*itParticle).velocity_;
		(*itParticle).currentTime_++;
	}

	camera_.SetMatView(followCamera_->GetCamera().GetMatView());

	//camera_.UpdateViewMatrix();
	camera_.UpdateCameraPos();
	pointLight_.Update();
	spotLight_.Update();
}

void GameScene::DrawBackGround(){

	

}

void GameScene::DrawModel(){

	

}

void GameScene::DrawParticleModel(){



}

void GameScene::DrawParticle(){

	if (sceneEvent_ == SceneEvent::Battle) {
		player_->DrawParticle(camera_);
		particle_->Draw(particles_, camera_);
	}
	

}

void GameScene::DrawUI(){

	if (sceneEvent_ == SceneEvent::Battle) {
		XButton_->Draw();
		char_Attack_->Draw();
	}
	if (sceneEvent_ == SceneEvent::PlayerDead) {
		gameOver_->Draw();
	}
	
}

void GameScene::DrawPostEffect() {

	outLine_->PreDrawScene(DirectXCommon::GetInstance()->GetCommandList());

	SkinningObject::preDraw();
	boss_->Draw(camera_);
	if (sceneEvent_ == SceneEvent::Battle) {
		player_->Draw(camera_);
	}

	outLine_->PostDrawScene(DirectXCommon::GetInstance()->GetCommandList());

	postEffect_->PreDrawScene(DirectXCommon::GetInstance()->GetCommandList());

	outLine_->Draw(DirectXCommon::GetInstance()->GetCommandList());

	BurnScars::preDraw();

	for (auto& burnScars : burnScarses_) {
		burnScars->Draw(camera_);
	}

	Object3d::preDraw();
	skydome_->Draw(camera_);
	ground_->Draw(camera_);
	
	
	for (const auto& elementBall : elementBalls_) {
		elementBall->Draw(camera_);
	}

	


	postEffect_->PostDrawScene(DirectXCommon::GetInstance()->GetCommandList());

}

void GameScene::DrawRenderTexture() {
	postEffect_->Draw(DirectXCommon::GetInstance()->GetCommandList());
}

void GameScene::BattleInit() {



}

void GameScene::BattleUpdate() {

	if (boss_->IsAttack() && elementBalls_.empty()) {
		boss_->ChangeBehavior(Boss::Behavior::kRoot);
	}

	for (std::list<std::unique_ptr<ElementBall>>::iterator itBall = elementBalls_.begin(); itBall != elementBalls_.end(); itBall++) {
		if (boss_->GetAction()!=Boss::Action::Attack && (*itBall)->GetPhase() == ElementBall::Phase::kShot) {
			boss_->ChangeAction(Boss::Action::Attack);
		}
	}

	player_->Update();
	boss_->Update();
	for (const auto& elementBall : elementBalls_) {
		elementBall->Update();
	}
	followCamera_->Update();

	for (auto& burnScars : burnScarses_) {
		burnScars->Update();
	}

	///衝突判定

	//プレイヤー攻撃とボス
	if (IsCollision(boss_->GetCollider(), player_->GetAttackCollider())) {
		player_->OnCollision();
		boss_->OnCollision();
	}

	//エレメントボールとプレイヤー
	for (const auto& elementBall : elementBalls_) {
		if (IsCollision(player_->GetCollider(), elementBall->GetCollider())) {
			player_->OnCollision();
			elementBall->OnCollision();
		}
	}

	for (const auto& elementBall : elementBalls_) {
		if (elementBall->IsLife()) {
			CreateBurnScars(elementBall->GetWorldPos());
		}
	}

	///

	if (player_->GetLife() == 0) {
		postEffect_->SetGrayScaleEffect(true);
		eventRequest_ = SceneEvent::PlayerDead;
	}

}

void GameScene::PlayerDeadInit() {

	elementBalls_.clear();
	alpha_ = 0.0f;

}

void GameScene::PlayerDeadUpdate() {
	alpha_ += 0.01f;
	alpha_ = std::min(alpha_, 1.0f);

	if (alpha_ >= 1.0f) {
		finishCount_--;
	}

	if (finishCount_ <= 0) {
		SceneManager::GetInstance()->ChangeScene("Title");
	}


	gameOver_->SetColor({ 1.0f,1.0f,1.0f,alpha_ });
}

void GameScene::DebugGUI(){
#ifdef _DEBUG
  
	ImGui::Begin("window");

	int count = finishCount_ / 60;
	ImGui::InputInt("ScenChangeCount", &count);
	
	ImGui::DragFloat2("XButton", &pos1.x, 1.0f);
	
	ImGui::DragFloat2("char_Attack", &pos2.x, 1.0f);
	

	ImGui::End();

	ImGui::Begin("Light");

	if (ImGui::TreeNode("PointLight")) {

		ImGui::ColorEdit4("color", &pointLight_.color_.x);
		ImGui::DragFloat3("position", &pointLight_.position_.x, 0.01f);
		ImGui::SliderFloat("intensity", &pointLight_.intensity_, 0.0f, 1.0f);
		ImGui::SliderFloat("radius", &pointLight_.radius_, 0.0f, 10.0f);
		ImGui::SliderFloat("decay", &pointLight_.decay_, 0.01f, 2.0f);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("SpotLight")) {

		ImGui::ColorEdit4("color", &spotLight_.color_.x);
		ImGui::DragFloat3("position", &spotLight_.position_.x, 0.01f);
		ImGui::SliderFloat("intensity", &spotLight_.intensity_, 0.0f, 1.0f);
		ImGui::SliderFloat3("direction", &spotLight_.direction_.x, -1.0f, 1.0f);
		ImGui::SliderFloat("distance", &spotLight_.distance_, 0.0f, 10.0f);
		ImGui::SliderFloat("decay", &spotLight_.decay_, 0.01f, 2.0f);
		ImGui::SliderFloat("cosAngle", &spotLight_.cosAngle_, 0.0f, spotLight_.cosFalloffStart_ - 0.001f);
		ImGui::SliderFloat("cosFalloffStart", &spotLight_.cosFalloffStart_, spotLight_.cosAngle_ + 0.001f, 1.0f);

		ImGui::TreePop();
	}

	ImGui::End();

#endif // _DEBUG
}

//Enemy* GameScene::EnemyPop(std::vector<std::shared_ptr<Model>> modelHandles, Vector3 pos) {
//
//	Enemy* enemy = new Enemy();
//	enemy->Init(modelHandles);
//	enemy->SetPos(pos);
//	std::uniform_int_distribution<int> distTime(5, 9);
//	enemy->SetCoolTime(distTime(randomEngine));
//	enemy->SetGameScene(this);
//
//	return enemy;
//}

//void GameScene::AddEnemyBullet(EnemyBullet* enemyBullet) {
//	enemyBullets_.push_back(std::unique_ptr<EnemyBullet>(enemyBullet));
//}

void GameScene::AddElementBall(ElementBall* elementBall) {
	elementBalls_.push_back(std::unique_ptr<ElementBall>(elementBall));
}

void GameScene::AddParticle(Particle::ParticleData particle) {
	particles_.push_back(particle);
}

void GameScene::CreateBurnScars(const Vector3& createPos) {

	auto& burnScars = burnScarses_.emplace_back(std::make_unique<BurnScars>());
	burnScars.reset(BurnScars::Create(burnScarsTex_));

	burnScars->position_ = createPos;
	burnScars->position_.y = 0.01f;

}
