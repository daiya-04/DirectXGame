#include "GameScene.h"

#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include <random>

GameScene::~GameScene() {}

void GameScene::Init(){

	camera_.Init();

#pragma region
	uint32_t playerModelHundle = ModelManager::Load("SeaHorse");
	playerModel_.reset(Object3d::Create(playerModelHundle));
	std::vector<Object3d*> playerModels = {
		playerModel_.get(),
	};
	player_ = std::make_unique<Player>();
	player_->Init(playerModels);
	player_->SetViewProjection(&camera_.GetViewProjection());
	camera_.SetTarget(&player_->GetWorldTransform());
#pragma endregion Player
#pragma region
	uint32_t floorModelHundle = ModelManager::Load("floor");
	floorModel_.reset(Object3d::Create(floorModelHundle));
	std::vector<Object3d*> planeModels = {
		floorModel_.get(),
	};
	floor_ = std::make_unique<Floor>();
	floor_->Init(planeModels);
	floor_->SetPos({0.0f,-2.0f,0.0f});
	floor_->SetScale({5.0f,1.0f,5.0f});
#pragma endregion Plane
}

void GameScene::Update(){
	DebugGUI();

	Input::GetInstance()->Update();

	camera_.Update();

	player_->Update();

	floor_->Update();

	if (IsCollision(player_->GetAABB(), floor_->GetAABB())) {
#ifdef _DEBUG
		ImGui::Begin("Hello");

		ImGui::End();
#endif
	}

}

void GameScene::DrawBackGround(){

}

void GameScene::DrawModel(){

	player_->Draw(camera_.GetViewProjection());

	floor_->Draw(camera_.GetViewProjection());
}

void GameScene::DrawParticleModel(){



}

void GameScene::DrawParticle(){

}

void GameScene::DrawUI(){



}

void GameScene::DebugGUI(){

	player_->ImGui();
}


