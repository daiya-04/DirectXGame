#include "GameScene.h"

#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include "Audio.h"
#include"SelectScene.h"
#include "Input.h"
#include <random>

GameScene::~GameScene() {}

float GameScene::timeNum_ = 0.0f;

void GameScene::Init() {

	camera_.Init();

	nowSceneNum_ = SelectScene::GetSelectNumber();

	if (nowSceneNum_ == 0) {
		levelData_ = std::unique_ptr<LevelData>(LevelLoader::LoadFile("beginner"));
	}
	else if (nowSceneNum_ == 1) {
		levelData_ = std::unique_ptr<LevelData>(LevelLoader::LoadFile("Stage1"));
	}
	else if (nowSceneNum_ == 2) {
		levelData_ = std::unique_ptr<LevelData>(LevelLoader::LoadFile("Stage2"));
	}
	else if (nowSceneNum_ == 3) {
		levelData_ = std::unique_ptr<LevelData>(LevelLoader::LoadFile("Stage3"));
	}
	
#pragma region
	Model* playerModelHundle = ModelManager::Load("InGameSeaHorse");
	Model* arrowModelHundle = ModelManager::Load("Line");
	playerModel_.reset(Object3d::Create(playerModelHundle));
	arrowModel_.reset(Object3d::Create(arrowModelHundle));
	std::vector<Object3d*> playerModels = {
		playerModel_.get(),
		arrowModel_.get(),
	};
	player_ = std::make_unique<Player>();
	player_->Init(playerModels);
	player_->SetViewProjection(&camera_.GetViewProjection());
	//player_->SetSoundHundle();
	camera_.SetTarget(&player_->GetWorldTransform());
	camera_.SetPlayer(player_.get());
#pragma endregion Player
#pragma region
	Model* sangoModelHundle = ModelManager::Load("sango");
	sangoModel_.reset(Object3d::Create(sangoModelHundle));
	std::vector<Object3d*> sangoModels = {
		sangoModel_.get(),
	};
#pragma endregion Sango
#pragma region
	Model* boxModelHundle = ModelManager::Load("Box");
	box_Model_.reset(Object3d::Create(boxModelHundle));
	std::vector<Object3d*> boxModels = {
		box_Model_.get(),
	};
	
#pragma endregion Box
#pragma region

	Model* goalModelHundle = ModelManager::Load("Goal");
	goal_Model_.reset(Object3d::Create(goalModelHundle));
	std::vector<Object3d*> goalModels = {
		goal_Model_.get(),
	};
	goal_ = std::make_unique<Goal>();
	goal_->Init(goalModels);
#pragma endregion Goal
#pragma region

	for (auto& objectData : levelData_->objects) {

		if (objectData.objectType == "Player") {
			player_->SetPos(objectData.translation);
			player_->SetRepopPos(objectData.translation);
		} else if (objectData.objectType == "sango") {
			/*Sango* newSango = new Sango();
			newSango->Init(sangoModels);
			newSango->SetPos(objectData.translation);*/

			auto& sang = sangoes_.emplace_back(std::make_unique<Sango>());
			sang->Init(sangoModels);
			sang->SetPos(objectData.translation);

			//sangoes_.push_back(std::unique_ptr<Sango>(newSango));
		} else if (objectData.objectType == "goal") {
			goal_->SetPos(objectData.translation);
		} else if (objectData.objectType == "floor") {
			Box* box = new Box();
			box->Init(boxModels);
			box->SetPos(objectData.translation);
			box->SetScale(objectData.scaling);

			boxes_.push_back(std::unique_ptr<Box>(box));
			
		}


	}

#pragma endregion 位置情報の読み込み
#pragma region 

	timeCounter_ = std::make_unique<TimeCounter>();
	timeCounter_->Init();
	timeCounter_->IsTimerAnable();
#pragma endregion タイマー
#pragma region 

	//スカイドーム
	Model* skyDomeModelHundle = ModelManager::Load("skyDome",false);
	SkyDomeModel_.reset(Object3d::Create(skyDomeModelHundle));
	world_.Init();
	world_.scale_ = {1000.0f,1000.0f,1000.0f};
	world_.UpdateMatrix();
#pragma endregion スカイドーム
#pragma region

	uint32_t UITex = TextureManager::Load("UI_grap.png");
	UI_Grap = std::make_unique<Sprite>(Sprite(UITex, { 700.0f,300.0f }, { 250.0f,80.0f }, 0.0f, { 0.0f,0.5f }, { 0.2f,0.2f,0.2f,1.0f }));
	UI_Grap->Initialize();
	UITex = TextureManager::Load("PressButton.png");
	UI_PlayerRoring = std::make_unique<Sprite>(Sprite(UITex, { 620.0f,310.0f }, { 508.0f,72.0f }, 0.0f, { 0.0f,0.5f }, { 0.2f,0.2f,0.2f,1.0f }));
	UI_PlayerRoring->Initialize();
	UITex = TextureManager::Load("ReleaseButton.png");
	UI_Release = std::make_unique<Sprite>(Sprite(UITex, { 620.0f,310.0f }, { 508.0f,72.0f }, 0.0f, { 0.0f,0.5f }, { 0.2f,0.2f,0.2f,1.0f }));
	UI_Release->Initialize();

	Model* signModelHundle = ModelManager::Load("Goal");
	sign_Model_.reset(Object3d::Create(signModelHundle));
	std::vector<Object3d*> signModels = {
		sign_Model_.get(),
	};
	signPost = std::make_unique<Signpost>();
	signPost->Init(signModels);
#pragma endregion 矢印
#pragma region
	ringParticle = std::make_unique<RingParticle>();
	ringParticle->Init();
#pragma endregion パーティクル
#pragma region
	std::vector<size_t>soundHandle;
	soundHandle.push_back(Audio::LoadWave("GrapJump.wav"));
	soundHandle.push_back(Audio::LoadWave("Rotation.wav"));
	player_->SetSoundHundle(soundHandle);
#pragma endregion
}

void GameScene::Update() {
	DebugGUI();

#pragma region
	camera_.Update();
	player_->Update();
	for (auto& sango : sangoes_) {
		sango->Update();
	}
	for (auto& box : boxes_) {
		box->Update();
	}
	goal_->Update();
	timeCounter_->Update();
	signPost->SetStert(player_->GetPosition());
	signPost->SetEnd(goal_->GetPosition());
	signPost->Update();

#ifdef _DEBUG
	ImGui::Begin("GameParticle");
	if (ImGui::Button("Addcircle1")) {
		ringParticle->addParticle(player_->GetPosition(),particleColor[0]);
	}
	if (ImGui::Button("Addcircle2")) {
		ringParticle->addParticle(player_->GetPosition(), particleColor[1]);
	}
	ImGui::End();
#endif
	ringParticle->Update();
#pragma endregion Update

#pragma region

	for (auto& sango : sangoes_) {

		if (IsCollision(player_->GetAABB(), sango->GetAABB())) {
				player_->HitSango();
			if (sango->GetIsAlreadyHit() == false) {
				sango->HitPlayer();
				player_->SetSangoId(sango->GetSangoId());
				player_->SetSangoPos(sango->GetPosition());

			}
		}else {
			sango->NotHitPlayer();
		}
	}
	for (auto& box : boxes_) {

		if (IsCollision(player_->GetAABB(), box->GetAABB())) {
			player_->hitBox(box->GetPosition(),box->GetColliderSize());
		}
	}
	if (IsCollision(player_->GetAABB(), goal_->GetAABB()) && IsGoal == false) {
		IsGoal = true;
		
		SelectScene::SetClearFlag(nowSceneNum_);
		timeCounter_->IsTimerStop();
		
		SceneManager::GetInstance()->ChangeScene(AbstractSceneFactory::SceneName::Result);
	}
#pragma endregion Collition

#pragma region
	if (!player_->GetgrapJumpFlag()) {
		if (player_->GetP_RoringFlag()) {
			IsRoringPlayer = true;
		}
		else if (!player_->GetP_RoringFlag()) {
			IsRoringPlayer = false;
			RoringparticleCount = 0;
		}
		if (IsRoringPlayer) {
			RoringparticleCount++;
			if (RoringparticleCount == 25 || RoringparticleCount == 40 || RoringparticleCount == 50 || RoringparticleCount == 55) {
				ringParticle->addCircleParticle(player_->GetPosition(), CircleparticleColor[1]);

			}
			if (RoringparticleCount == 55) {
				RoringparticleCount = 45;
			}
		}
	}
	if (player_->GetP_AutoGrapFlag()) {
		IsAutoGrapPlayer = true;
	}
	if (IsAutoGrapPlayer) {
		AutoGrapparticleCount++;
	}
	if (AutoGrapparticleCount == MaxCount) {
		ringParticle->addCircleParticle(player_->GetPosition(), CircleparticleColor[0]);
		ringParticle->addParticle(player_->GetPosition(), CircleparticleColor[1]);
		AutoGrapparticleCount = 0;
		IsAutoGrapPlayer = false;
	}
#pragma endregion パーティクル

	timeNum_ = timeCounter_->GetNumberCount();
}

void GameScene::DrawBackGround() {

}

void GameScene::DrawModel() {

	player_->Draw(camera_.GetViewProjection());

	for (auto& sango : sangoes_) {
		sango->Draw(camera_.GetViewProjection());
	}
	for (auto& box : boxes_) {
		box->Draw(camera_.GetViewProjection());
	}

	goal_->Draw(camera_.GetViewProjection());

	SkyDomeModel_->Draw(world_,camera_.GetViewProjection());

	signPost->Draw(camera_.GetViewProjection());
}

void GameScene::DrawParticleModel() {



}

void GameScene::DrawParticle() {
	ringParticle->Draw(camera_.GetViewProjection());
}

void GameScene::DrawUI() {

	timeCounter_->Draw();
	if (player_->GetFarstFlag()&& player_->GetCanGrapFlag()) {
		UI_Grap->Draw();
	}
	if (!player_->GetFarstFlag() && player_->GetCanGrapFlag()) {
		if (player_->GetMaxPower()){
			UI_Release->Draw();
		}
		else {
			UI_PlayerRoring->Draw();
		}
		
	}

}

void GameScene::DebugGUI() {

	player_->ImGui();
	camera_.ImGui();
#ifdef _DEBUG
	ImGui::Begin("Timer");
	if (ImGui::Button("On")) {
		timeCounter_->IsTimerAnable();
	}
	if (ImGui::Button("Off")) {
		timeCounter_->IsTimerStop();
	}
	if (ImGui::Button("Reset")) {
		timeCounter_->Reset();
	}
	ImGui::End();
#endif
}


