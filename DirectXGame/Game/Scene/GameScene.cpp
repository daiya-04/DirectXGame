#include "GameScene.h"

#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include <random>

GameScene::~GameScene() {}

void GameScene::Init(){

	camera_.Init();
	pointLight_.Init();

	uint32_t circle = TextureManager::Load("circle.png");

	Model_ = ModelManager::Load("plane");
	Model2_ = ModelManager::Load("teapot");
	terrainModel_ = ModelManager::Load("terrain");

	obj_.reset(Object3d::Create(Model_));
	objWT_.Init();

	obj2_.reset(Object3d::Create(Model2_));
	objWT2_.Init();

	terrain_.reset(Object3d::Create(terrainModel_));
	terrainWT_.Init();
	terrainWT_.rotation_.y = -1.57f;
	terrain_->SetPointLight(&pointLight_);

	particle_ = std::make_unique<Particle>();
	particle_.reset(Particle::Create(circle, 100));

	emitter_.count_ = 5;
	emitter_.frequency_ = 0.5f;
	
}

void GameScene::Update(){
	DebugGUI();

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


	camera_.UpdateViewMatrix();
	objWT_.UpdateMatrix();
	objWT2_.UpdateMatrix();
	terrainWT_.UpdateMatrix();
	pointLight_.Update();
}

void GameScene::DrawBackGround(){

	

}

void GameScene::DrawModel(){

	//obj_->Draw(objWT_, camera_);
	//obj2_->Draw(objWT2_, camera_);
	terrain_->Draw(terrainWT_, camera_);

}

void GameScene::DrawParticleModel(){



}

void GameScene::DrawParticle(){

	//particle_->Draw(particleData_, camera_);

}

void GameScene::DrawUI(){



}

void GameScene::DebugGUI(){
#ifdef _DEBUG

	ImGui::Begin("object");

	ImGui::DragFloat3("plane transform", &objWT_.translation_.x, 0.01f);
	ImGui::DragFloat3("teapot transform", &objWT2_.translation_.x, 0.01f);
	ImGui::DragFloat3("teapot scale", &objWT2_.scale_.x, 0.01f);

	ImGui::End();

	ImGui::Begin("Particle");

	ImGui::DragFloat3("emitter transform", &emitter_.translate_.x, 0.01f);
	ImGui::SliderFloat("frequency", &emitter_.frequency_, 0.0f, 5.0f);
	ImGui::InputInt("create count", reinterpret_cast<int*>(&emitter_.count_));


	ImGui::End();

	ImGui::Begin("camera");

	ImGui::DragFloat3("pos", &camera_.translation_.x, 0.01f);
	ImGui::DragFloat3("rotate", &camera_.rotation_.x, 0.01f);

	ImGui::End();

	ImGui::Begin("PointLight");

	ImGui::ColorEdit4("color", &pointLight_.color_.x);
	ImGui::DragFloat3("position", &pointLight_.position_.x, 0.01f);
	ImGui::SliderFloat("intensity", &pointLight_.intensity_, 0.0f, 1.0f);
	ImGui::SliderFloat("radius", &pointLight_.radius_, 0.0f, 10.0f);
	ImGui::SliderFloat("decay", &pointLight_.decay_, 0.01f, 2.0f);

	ImGui::End();

#endif // _DEBUG
}


