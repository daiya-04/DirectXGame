#include "GameScene.h"

#include "../SceneManager.h"
#include "../../../ImGuiManager.h"

void GameScene::Initialize()
{
	maingCamera_.reset(new ViewProjection());

	maingCamera_->translation_ = kOriginOffset_;
	maingCamera_->rotation_ = kOriginAngle;

	currentStage_.reset(new DebugStage);
	currentStage_->Initialize(maingCamera_.get());

	

	

}

void GameScene::Reset()
{
	//viewProjection_.reset(new ViewProjection());
	maingCamera_->Initialize();
	maingCamera_->translation_ = kOriginOffset_;
	maingCamera_->rotation_ = kOriginAngle;

	Audio::GetInstance()->SoundPlayLoopEnd(playHandle_);
	size_t bgmHandle = Audio::GetInstance()->SoundLoadWave("inGameBGM.wav");
	playHandle_ = Audio::GetInstance()->SoundPlayLoopStart(bgmHandle);
	Audio::GetInstance()->SetValume(playHandle_, 0.4f);

	currentStage_->SetStageNum(stageNum_);
	currentStage_->Reset();

}

void GameScene::Update()
{
	DebugGUI();

	currentStage_->Update();

	if (input_->TriggerKey(DIK_SPACE))
	{
		SceneManager::GetInstace()->ChegeScene(kTITLE);
	}
	// カメラ移動

}

void GameScene::DrawModel()
{
	currentStage_->Draw();
}

void GameScene::DrawUI()
{

}

GameScene::~GameScene() {}

void GameScene::DebugGUI()
{
#ifdef _DEBUG

	ImGui::Begin("GameScene");

	ImGui::DragFloat3("ViewRotate", &maingCamera_->rotation_.x, 0.01f);
	ImGui::DragFloat3("ViewTranslate", &maingCamera_->translation_.x, 0.1f);

	if (ImGui::Button("Reset"))
	{
		Reset();
	}

	maingCamera_->UpdateMatrix();

	ImGui::End();

#endif // _DEBUG

}
