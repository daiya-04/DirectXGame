#include "GameScene.h"

#include "../SceneManager.h"
#include "../../../ImGuiManager.h"

void GameScene::Initialize()
{
	viewProjection_.reset(new ViewProjection());
	viewProjection_->Initialize();

	player_.reset(new Player());
	player_->Initialize();
	IObject::StaticSetViewProjection(viewProjection_.get());
}

void GameScene::Update()
{
	player_->Update();

	DebugGUI();

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

void GameScene::DebugGUI()
{
#ifdef _DEBUG

	ImGui::Begin("StageManager");
	
	ImGui::DragFloat3("Rotate", &viewProjection_->rotation_.x, 0.01f);
	ImGui::DragFloat3("Translate", &viewProjection_->translation_.x, 0.1f);

	viewProjection_->UpdateMatrix();

	ImGui::End();

#endif // _DEBUG

}
