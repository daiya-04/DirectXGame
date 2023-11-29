#include "GameScene.h"

#include "../SceneManager.h"
#include "../../../ImGuiManager.h"
#include <cmath>
#include <numbers>

void GameScene::Initialize()
{
	maingCamera_.reset(new ViewProjection());

	maingCamera_->translation_ = kOriginOffset_;
	maingCamera_->rotation_ = kOriginAngle;

	uint32_t clearParticleHandle_ = TextureManager::Load("star.png");
	clearParticle_ = std::make_unique<Particle>();
	clearParticle_.reset(Particle::Create(clearParticleHandle_, 16));

#ifdef _DEBUG

	currentStage_.reset(new DebugStage);
	currentStage_->Initialize(maingCamera_.get());

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

	if (input_->TriggerKey(DIK_ESCAPE))
	{
		SceneManager::GetInstace()->ChegeScene(kSELECT);
	}
	// カメラ移動



	//ClearParticle
	if (MapManager::GetInstance()->IsClear())
	{
		if (clearParticles_.empty())
		{

			// ステージの中央を取得
			size_t stageSize = currentStage_->GetStageSize();
			float stageCenter = (float)stageSize / 2.0f * BaseBlock::kBlockSize - BaseBlock::kBlockSize / 2.0f;

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
		//SceneManager::GetInstace()->ChegeScene(kSELECT);
	}

}

void GameScene::DrawModel()
{
	currentStage_->Draw();
}

void GameScene::DrawUI()
{

}

void GameScene::DrawParticle()
{
	clearParticle_->Draw(clearParticles_, *maingCamera_.get());
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
