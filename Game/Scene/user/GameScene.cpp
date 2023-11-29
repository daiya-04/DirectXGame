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

	uint32_t clearParticleHandle_ = TextureManager::Load("circle.png");
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

	if (input_->TriggerKey(DIK_SPACE))
	{
		SceneManager::GetInstace()->ChegeScene(kTITLE);
	}
	// カメラ移動



	//ClearParticle
	if (MapManager::GetInstance()->IsClear()) {
		if (clearParticles_.empty()) {
			for (size_t index = 0; index < clearParticle_->particleMaxNum_; index++) {
				Particle::ParticleData particle;
				//particle.worldTransform_.translation_ = {0.0f,3.0f,0.0f};
				particle.worldTransform_.translation_ = MapManager::GetInstance()->GetPlayerPos();
				Vector3 velocityDir = { 0.0f,0.0f,1.0f };
				particle.velocity_ = fTransform(velocityDir, MakeRotateYMatrix(index * 22.5f * (std::numbers::pi_v<float> / 180.0f))) * 5.0f;
				particle.lifeTime_ = 2.0f;
				particle.color_ = { 1.0f,1.0f,1.0f,1.0f };
				particle.currentTime_ = 0.0f;
				clearParticles_.push_back(particle);
			}
		}
		const float kDeltaTime = 1.0f / 60.0f;
		for (std::list<Particle::ParticleData>::iterator itParticle = clearParticles_.begin(); itParticle != clearParticles_.end(); itParticle++) {
			(*itParticle).worldTransform_.translation_ += (*itParticle).velocity_ * kDeltaTime;
			(*itParticle).currentTime_ += kDeltaTime;
			if ((*itParticle).currentTime_ >= (*itParticle).lifeTime_) {
				SceneManager::GetInstace()->ChegeScene(kSELECT);
			}
		}
	}
	else {
		for (std::list<Particle::ParticleData>::iterator itParticle = clearParticles_.begin(); itParticle != clearParticles_.end(); itParticle++) {
			(*itParticle).currentTime_ = (*itParticle).lifeTime_;
		}
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
