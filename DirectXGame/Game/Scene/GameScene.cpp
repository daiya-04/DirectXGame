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
	Model* floorModelHundle = ModelManager::Load("floor");
	floorModel_.reset(Object3d::Create(floorModelHundle));
	std::vector<Object3d*> planeModels = {
		floorModel_.get(),
	};
	floor_ = std::make_unique<Floor>();
	floor_->Init(planeModels);
	floor_->SetPos({ 0.0f,0.0f,0.0f });
	floor_->SetScale({ 10.0f,1.0f,10.0f });
#pragma endregion Plane
#pragma region
	Model* sangoModelHundle = ModelManager::Load("sango");
	sangoModel_.reset(Object3d::Create(sangoModelHundle));
	std::vector<Object3d*> sangoModels = {
		sangoModel_.get(),
	};
	/*sango_ = std::make_unique<Sango>();
	sango_->Init(sangoModels);
	sango_->SetPos({0.0f,2.0f,0.0f});
	sango2_ = std::make_unique<Sango>();
	sango2_->Init(sangoModels);
	sango2_->SetPos({10.0f,10.0f,0.0f});
	sango3_ = std::make_unique<Sango>();
	sango3_->Init(sangoModels);
	sango3_->SetPos({ 0.0f,15.0f,0.0f });
	sango4_ = std::make_unique<Sango>();
	sango4_->Init(sangoModels);
	sango4_->SetPos({ 10.0f,20.0f,0.0f });*/
#pragma endregion Sango
#pragma region

#pragma endregion Box

#pragma region

	for (auto& objectData : levelData_->objects) {

		if (objectData.objectType == "Player") {
			player_->SetPos(objectData.translation);
		}else if (objectData.objectType == "sango") {
			Sango* newSango = new Sango();
			newSango->Init(sangoModels);
			newSango->SetPos(objectData.translation);

			sangoes_.push_back(std::unique_ptr<Sango>(newSango));
		}
		

	}

#pragma endregion 位置情報の読み込み

	Model* goalModelHundle = ModelManager::Load("Goal");
	goal_Model_.reset(Object3d::Create(goalModelHundle));
	std::vector<Object3d*> goalModels = {
		goal_Model_.get(),
	};
	goal_ = std::make_unique<Goal>();
	goal_->Init(goalModels);
	goal_->SetPos({15.0f,1.0f,0.0f});
#pragma endregion Goal
}

void GameScene::Update() {
	DebugGUI();

	camera_.Update();

	player_->Update();

	floor_->Update();

	for (auto& sango : sangoes_) {
		sango->Update();
	}

	goal_->Update();

#pragma region 
	if (IsCollision(player_->GetAABB(), floor_->GetAABB())) {
#ifdef _DEBUG
		ImGui::Begin("Floor");
		ImGui::End();
#endif
		player_->HitFloor(floor_->GetPosition().y);
	}

	for (auto& sango : sangoes_) {

		if (IsCollision(player_->GetAABB(), sango->GetAABB())) {
			player_->HitSango(sango->GetPosition());
			if (sango->GetIsAlreadyHit() == false) {
				sango->HitPlayer();
				player_->SetSangoId(sango->GetSangoId());
			}
		}else {
			sango->NotHitPlayer();
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

	floor_->Draw(camera_.GetViewProjection());

	for (auto& sango : sangoes_) {
		sango->Draw(camera_.GetViewProjection());
	}

	goal_->Draw(camera_.GetViewProjection());

}

void GameScene::DrawParticleModel() {



}

void GameScene::DrawParticle() {

}

void GameScene::DrawUI() {



}

void GameScene::DebugGUI() {

	player_->ImGui();
}


