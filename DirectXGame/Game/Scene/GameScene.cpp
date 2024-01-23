#include "GameScene.h"

#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include <random>

GameScene::~GameScene() {}

void GameScene::Init(){

	camera_.Init();

#pragma region
	uint32_t playerModelHundle = ModelManager::Load("InGameSeaHorse");
	uint32_t arrowModelHundle = ModelManager::Load("Line");
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
	uint32_t floorModelHundle = ModelManager::Load("floor");
	floorModel_.reset(Object3d::Create(floorModelHundle));
	std::vector<Object3d*> planeModels = {
		floorModel_.get(),
	};
	floor_ = std::make_unique<Floor>();
	floor_->Init(planeModels);
	floor_->SetPos({0.0f,0.0f,0.0f});
	floor_->SetScale({10.0f,1.0f,10.0f});
#pragma endregion Plane
#pragma region
	uint32_t sangoModelHundle = ModelManager::Load("sango");
	sangoModel_.reset(Object3d::Create(sangoModelHundle));
	std::vector<Object3d*> sangoModels = {
		sangoModel_.get(),
	};
	sango_ = std::make_unique<Sango>();
	sango_->Init(sangoModels);
	sango_->SetPos({0.0f,2.0f,0.0f});
	sango2_ = std::make_unique<Sango>();
	sango2_->Init(sangoModels);
	sango2_->SetPos({10.0f,10.0f,0.0f});
#pragma endregion Sango
}

void GameScene::Update(){
	DebugGUI();

	camera_.Update();

	player_->Update();

	floor_->Update();

	sango_->Update();
	sango2_->Update();


#pragma region 
	if (IsCollision(player_->GetAABB(), floor_->GetAABB())) {
#ifdef _DEBUG
		ImGui::Begin("Floor");
		ImGui::End();
#endif
		player_->HitFloor(floor_->GetPosition().y);
	}
	if (IsCollision(player_->GetAABB(), sango_->GetAABB())) {
#ifdef _DEBUG
		ImGui::Begin("Sango");
		ImGui::End();
#endif
		player_->HitSango(sango_->GetPosition());
		if (sango_->GetIsAlreadyHit() == false) {
			sango_->HitPlayer();
			player_->SetSangoId(sango_->GetSangoId());
		}
	}
	else {
		sango_->NotHitPlayer();
	}
	if (IsCollision(player_->GetAABB(), sango2_->GetAABB())) {
#ifdef _DEBUG
		ImGui::Begin("Sango");
		ImGui::End();
#endif
		player_->HitSango(sango2_->GetPosition());
		if (sango2_->GetIsAlreadyHit() == false) {
			sango2_->HitPlayer();
			player_->SetSangoId(sango2_->GetSangoId());
		}
	}
	else {
		sango2_->NotHitPlayer();
	}
#pragma endregion 当たり判定
}

void GameScene::DrawBackGround(){

}

void GameScene::DrawModel(){

	player_->Draw(camera_.GetViewProjection());

	floor_->Draw(camera_.GetViewProjection());

	sango_->Draw(camera_.GetViewProjection());
	sango2_->Draw(camera_.GetViewProjection());
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


