#include "GameScene.h"

#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include "Audio.h"
#include "Input.h"
#include <random>

GameScene::~GameScene() {}

void GameScene::Init() {

	camera_.Init();

	levelData_ = std::unique_ptr<LevelData>(LevelLoader::LoadFile("beginner"));

	

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
		} else if (objectData.objectType == "sango") {
			Sango* newSango = new Sango();
			newSango->Init(sangoModels);
			newSango->SetPos(objectData.translation);

			sangoes_.push_back(std::unique_ptr<Sango>(newSango));
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

	timeCounter_ = std::make_unique<TimeCounter>();
	timeCounter_->Init();
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
		SceneManager::GetInstance()->ChangeScene(AbstractSceneFactory::SceneName::Select);
	}
  
#pragma endregion 当たり判定
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

}

void GameScene::DrawParticleModel() {



}

void GameScene::DrawParticle() {

}

void GameScene::DrawUI() {

	timeCounter_->Draw();

}

void GameScene::DebugGUI() {

	player_->ImGui();

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


