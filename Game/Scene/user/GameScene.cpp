#include "GameScene.h"

#include "../SceneManager.h"
#include "../../../ImGuiManager.h"

void GameScene::Initialize()
{
	viewProjection_.reset(new ViewProjection());



	debugStage_.reset(new DebugStage);
	debugStage_->Initialize();

	Reset();

}

void GameScene::Reset()
{
	//viewProjection_.reset(new ViewProjection());
	viewProjection_->Initialize();
	viewProjection_->translation_ = { 0.0f,17.0f,-10.0f };
	viewProjection_->rotation_ = { 1.0f,0.0f,0.0f, };

	debugStage_->Reset();

}

void GameScene::Update()
{
	DebugGUI();

	if (input_->TriggerKey(DIK_SPACE)) {
		SceneManager::GetInstace()->ChegeScene(kTITLE);
	}

}

void GameScene::DrawModel()
{
}

void GameScene::DrawUI()
{

}

GameScene::~GameScene() {}

void GameScene::DebugGUI()
{
#ifdef _DEBUG

	ImGui::Begin("DebugStage");

	ImGui::DragFloat3("ViewRotate", &viewProjection_->rotation_.x, 0.01f);
	ImGui::DragFloat3("ViewTranslate", &viewProjection_->translation_.x, 0.1f);

	viewProjection_->UpdateMatrix();

	ImGui::End();

#endif // _DEBUG

}
