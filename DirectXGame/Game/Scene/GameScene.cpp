#include "GameScene.h"

#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include <random>

GameScene::~GameScene() {}

void GameScene::Init(){

	camera_.Init();

	uint32_t circle = TextureManager::Load("circle.png");

	Model_ = ModelManager::Load("plane");
	Model2_ = ModelManager::Load("teapot");

	obj_.reset(Object3d::Create(Model_));
	objWT_.Init();

	obj2_.reset(Object3d::Create(Model2_));
	objWT2_.Init();

	particle_ = std::make_unique<Particle>();
	particle_.reset(Particle::Create(circle, 100));

	emitter_.count_ = 5;
	emitter_.frequency_ = 0.5f;

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
#pragma endregion
}

void GameScene::Update(){
	DebugGUI();

	Input::GetInstance()->Update();

	std::random_device seedGenerator;
	std::mt19937 randomEngine(seedGenerator());

	emitter_.frequencyTime_ += kDeltaTime;
	if (emitter_.frequency_ <= emitter_.frequencyTime_) {
		particleData_.splice(particleData_.end(), Particle::Emit(emitter_, randomEngine));
		emitter_.frequencyTime_ -= emitter_.frequency_;
	}
	for (std::list<Particle::ParticleData>::iterator itParticle = particleData_.begin(); itParticle != particleData_.end(); itParticle++) {
		(*itParticle).worldTransform_.translation_ += (*itParticle).velocity_ * kDeltaTime;
		(*itParticle).currentTime_ += kDeltaTime;
	}


	camera_.Update();

	player_->Update();

	objWT_.UpdateMatrix();
	objWT2_.UpdateMatrix();

}

void GameScene::DrawBackGround(){

	

}

void GameScene::DrawModel(){
	obj_->Draw(objWT_, camera_.GetViewProjection());
	obj2_->Draw(objWT2_, camera_.GetViewProjection());

	player_->Draw(camera_.GetViewProjection());
}

void GameScene::DrawParticleModel(){



}

void GameScene::DrawParticle(){

	particle_->Draw(particleData_, camera_.GetViewProjection());

}

void GameScene::DrawUI(){



}

void GameScene::DebugGUI(){

	player_->ImGui();
}


