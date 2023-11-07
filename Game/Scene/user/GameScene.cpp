#include "GameScene.h"

#include "../SceneManager.h"

void GameScene::Initialize()
{
	viewProjection_.reset(new ViewProjection());
	viewProjection_->Initialize();

	player_.reset(new Player());
	player_->Initialize();
	IObject::SetViewProjection(viewProjection_.get());
}

void GameScene::Update()
{
	player_->Update();

	if (input_->TriggerKey(DIK_SPACE)) {
		SceneManager::GetInstace()->ChegeScene(kTITLE);
	}

}

void GameScene::DrawModel()
{
	player_->Draw();
}

void GameScene::DrawUI()
{

}

GameScene::~GameScene() {}