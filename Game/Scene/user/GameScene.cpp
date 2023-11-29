#include "GameScene.h"

#include "../SceneManager.h"
#include "../../../ImGuiManager.h"
#include <cmath>
#include <numbers>

using MoveDirect = MapManager::MoveDirect;

void GameScene::Initialize()
{
	stageCamera_ = std::make_unique<StageCamera>();
	stageCamera_->Initialize();
	maingCamera_ = stageCamera_->GetViewProjection();
	maingCamera_->Initialize();

	/*maingCamera_->translation_ = kOriginOffset_;
	maingCamera_->rotation_ = kOriginAngle;*/

	uint32_t clearParticleHandle_ = TextureManager::Load("star.png");
	clearParticle_ = std::make_unique<Particle>();
	clearParticle_.reset(Particle::Create(clearParticleHandle_, 16));


	uint32_t movePlayerKeyHandle = TextureManager::Load("ui1.png");
	uint32_t moveCameraKeyHandle = TextureManager::Load("cameraUi.png");
	
	clearConditionHandle[0] = TextureManager::Load("condition1.png");
	clearConditionHandle[1] = TextureManager::Load("condition2.png");
	
	playerMadeHandle[0] = TextureManager::Load("moveMode.png");
	playerMadeHandle[1] = TextureManager::Load("tobashiMode.png");
	uint32_t spaceKeyHandle = TextureManager::Load("space.png");
	
	stageNameHandle[0] = TextureManager::Load("stage1.png");
	stageNameHandle[1] = TextureManager::Load("stage2.png");
	stageNameHandle[2] = TextureManager::Load("stage3.png");
	stageNameHandle[3] = TextureManager::Load("stage4.png");
	stageNameHandle[4] = TextureManager::Load("stage5.png");
	stageNameHandle[5] = TextureManager::Load("stage6.png");
	stageNameHandle[6] = TextureManager::Load("stage7.png");
	stageNameHandle[7] = TextureManager::Load("stage8.png");

	movePlayerKey_.reset(new Sprite(movePlayerKeyHandle, {120.0f,640.0f}, { 200.0f,120.0f }, 0.0f, { 0.5f,0.5f }));
	movePlayerKey_->Initialize();

	moveCameraKey_.reset(new Sprite(moveCameraKeyHandle, {1160.0f,640.0f}, { 200.0f,120.0f }, 0.0f, { 0.5f,0.5f }));
	moveCameraKey_->Initialize();

	clearCondition_.reset(new Sprite(clearConditionHandle[0], {640.0f,625.0f}, { 768.0f,150.0f }, 0.0f, { 0.5f,0.5f }));
	clearCondition_->Initialize();

	playerMode_.reset(new Sprite(playerMadeHandle[0], {1105.0f,50.0f}, { 350.0f,100.0f }, 0.0f, { 0.5f,0.5f }));
	playerMode_->Initialize();

	spaceKey_.reset(new Sprite(spaceKeyHandle, {1105.0f,125.0f}, { 150.0f,50.0f }, 0.0f, { 0.5f,0.5f }));
	spaceKey_->Initialize();

	stageName_.reset(new Sprite(stageNameHandle[0], {200.0f,50.0f}, { 400.0f,100.0f }, 0.0f, { 0.5f,0.5f }));
	stageName_->Initialize();

#ifdef _DEBUG

	currentStage_.reset(new DebugStage);
	currentStage_->Initialize(maingCamera_);

#endif // _DEBUG

#ifndef _DEBUG

	currentStage_.reset(new Stage);
	currentStage_->Initialize(maingCamera_.get());

#endif // !_DEBUG


	//Reset();

}

void GameScene::Reset()
{
	//viewProjection_.reset(new ViewProjection());
	maingCamera_->Initialize();
	//maingCamera_->translation_ = kOriginOffset_;
	//maingCamera_->rotation_ = kOriginAngle;

	stageCamera_->Initialize();

	Audio::GetInstance()->SoundPlayLoopEnd(playHandle_);
	size_t bgmHandle = Audio::GetInstance()->SoundLoadWave("inGameBGM.wav");
	playHandle_ = Audio::GetInstance()->SoundPlayLoopStart(bgmHandle);
	Audio::GetInstance()->SetValume(playHandle_, 0.4f);

	currentStage_->SetStageNum(stageNum_);
	currentStage_->Reset();

	gameOverStagingTime_ = 0;

	movePlayerKey_->Initialize();
	moveCameraKey_->Initialize();
	clearCondition_->Initialize();
	playerMode_->Initialize();
	spaceKey_->Initialize();
	stageName_->Initialize();

	stageName_->SetTextureHandle(stageNameHandle[static_cast<size_t>(stageNum_)]);
	if (stageNum_ >= 4) {
		clearCondition_->SetTextureHandle(clearConditionHandle[1]);
	}else {
		clearCondition_->SetTextureHandle(clearConditionHandle[0]);
	}
	

}

void GameScene::Update()
{
	DebugGUI();


	if (input_->TriggerKey(DIK_ESCAPE))
	{
		SceneManager::GetInstace()->ChegeScene(kSELECT);
	}


	if (stageNum_ >= 3) {
		if (input_->TriggerKey(DIK_SPACE)) {
			if (isShot_) {
				isShot_ = false;
				playerMode_->SetTextureHandle(playerMadeHandle[0]);
			}
			else {
				isShot_ = true;
				playerMode_->SetTextureHandle(playerMadeHandle[1]);
			}
		}
	}
	
	if (input_->TriggerKey(DIK_R))
	{
		Reset();
	}
  
	// カメラ移動

	// ステージの中央を取得
	size_t stageSize = currentStage_->GetStageSize();
	float stageCenter = (float)stageSize / 2.0f * BaseBlock::kBlockSize - BaseBlock::kBlockSize / 2.0f;

	stageCamera_->Update(stageCenter);

	//maingCamera_->matView_ = stageCamera_->GetViewProjection()->matView_;


	currentStage_->SetCameraDirection(CameraDirection());
	currentStage_->Update();

	//ClearParticle
	if (MapManager::GetInstance()->IsClear())
	{
		if (clearParticles_.empty())
		{



			for (size_t index = 0; index < clearParticle_->particleMaxNum_; index++)
			{
				Particle::ParticleData particle;
				//particle.worldTransform_.translation_ = {0.0f,3.0f,0.0f};
				// ステージの中央から出す
				particle.worldTransform_.translation_ = { stageCenter,-stageCenter,stageCenter };
				particle.worldTransform_.scale_ = { 0.5f,0.5f,0.5f };
				Vector3 velocityDir = { 0.0f,0.0f,1.0f };
				particle.velocity_ = fTransform(velocityDir, MakeRotateYMatrix(index * 22.5f * (std::numbers::pi_v<float> / 180.0f))) * 5.0f;
				particle.lifeTime_ = 2.0f;
				particle.color_ = { 1.0f,1.0f,0.0f,1.0f };
				particle.currentTime_ = 0.0f;
				clearParticles_.push_back(particle);
			}
		}
		const float kDeltaTime = 1.0f / 60.0f;
		for (std::list<Particle::ParticleData>::iterator itParticle = clearParticles_.begin(); itParticle != clearParticles_.end(); itParticle++)
		{
			(*itParticle).worldTransform_.translation_ += (*itParticle).velocity_ * kDeltaTime;
			(*itParticle).worldTransform_.rotation_.z += 0.2f;
			(*itParticle).currentTime_ += kDeltaTime;
			if ((*itParticle).currentTime_ >= (*itParticle).lifeTime_)
			{
				SceneManager::GetInstace()->ChegeScene(kSELECT);
				
			}
		}
	}
	else
	{
		for (std::list<Particle::ParticleData>::iterator itParticle = clearParticles_.begin(); itParticle != clearParticles_.end(); itParticle++)
		{
			(*itParticle).currentTime_ = (*itParticle).lifeTime_;
		}
	}
	// ゲームオーバー時の演出
	if (MapManager::GetInstance()->IsGameOVer())
	{
		gameOverStagingTime_++;
		if (cGameOverStagingTime_ < gameOverStagingTime_)
		{
			SceneManager::GetInstace()->ChegeScene(kSELECT);
		}
	}

	
	

}

void GameScene::DrawModel()
{
	currentStage_->Draw();
}

void GameScene::DrawUI()
{
	moveCameraKey_->Draw();
	movePlayerKey_->Draw();
	clearCondition_->Draw();
	if (stageNum_ >= 3) {
		playerMode_->Draw();
		spaceKey_->Draw();
	}
	stageName_->Draw();
}

void GameScene::DrawParticle()
{
	clearParticle_->Draw(clearParticles_, *maingCamera_);
}

GameScene::~GameScene() {}

void GameScene::DebugGUI()
{
#ifdef _DEBUG

	ImGui::Begin("GameScene");

	ImGui::DragFloat3("ViewRotate", &maingCamera_->rotation_.x, 0.01f);
	ImGui::DragFloat3("ViewTranslate", &maingCamera_->translation_.x, 0.1f);
	ImGui::Text("%d", gameOverStagingTime_);


	ImGui::Text("%d", CameraDirection());

	if (ImGui::Button("Reset"))
	{
		Reset();
	}

	maingCamera_->UpdateMatrix();

	ImGui::End();

#endif // _DEBUG

}

MapManager::MoveDirect GameScene::CameraDirection()
{
	// カメラの方向から今カメラがいる方向を算出
	float rotateY = stageCamera_->GetViewProjection()->rotation_.y;
	float pi4 = static_cast<float>(std::numbers::pi) / 4.0f;
	MoveDirect cameraDirect = MoveDirect::dFRONT;
	if (-pi4 < rotateY && rotateY < pi4)
	{
		cameraDirect = MoveDirect::dFRONT;
	}
	else if (pi4 * 3 < rotateY && rotateY < pi4 * 5 ||
		-pi4 * 5 < rotateY && rotateY < -pi4 * 3)
	{
		cameraDirect = MoveDirect::dBACK;
	}
	else if (pi4 * 5 < rotateY && rotateY < pi4 * 7||
		-pi4 * 3 < rotateY && rotateY < -pi4 * 1)
	{
		cameraDirect = MoveDirect::dRIGHT;
	}
	else if (pi4 * 1 < rotateY && rotateY < pi4 * 3 ||
		-pi4 * 7 < rotateY && rotateY < -pi4 * 5)
	{
		cameraDirect = MoveDirect::dLEFT;
	}
	return cameraDirect;
}
