#include "GameScene.h"

#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include "Audio.h"
#include "Input.h"
#include <random>

GameScene::~GameScene() {}

void GameScene::Init(){

	camera_.Init();
	pointLight_.Init();
	spotLight_.Init();
	for (auto& wt : WTs_) {
		wt.Init();
	}

	Object3d::SetPointLight(&pointLight_);
	Object3d::SetSpotLight(&spotLight_);

	uint32_t circle = TextureManager::Load("circle.png");

	Model_ = ModelManager::Load("plane");
	Model2_ = ModelManager::Load("teapot");
	terrainModel_ = ModelManager::Load("terrain");

	levelData_ = std::unique_ptr<LevelData>(LevelLoader::LoadFile("stage"));

	//レベルデータからオブジェクトを生成、配置
	for (auto& objectData : levelData_->objects) {
		Model* model = ModelManager::Load(objectData.fileName);

		Object3d* newObject = Object3d::Create(model);

		WorldTransform wt;
		wt.Init();

		wt.translation_ = objectData.translation;
		wt.rotation_ = objectData.rotation;
		wt.scale_ = objectData.scaling;

		WTs_.push_back(wt);
		objects_.push_back(std::unique_ptr<Object3d>(newObject));
	}

	obj_.reset(Object3d::Create(Model2_));
	objWT_.Init();

	obj2_.reset(Object3d::Create(Model2_));
	objWT2_.Init();

	terrain_.reset(Object3d::Create(terrainModel_));
	terrainWT_.Init();
	terrainWT_.rotation_.y = -1.57f;
	

	particle_ = std::make_unique<Particle>();
	particle_.reset(Particle::Create(circle, 100));

	emitter_.count_ = 5;
	emitter_.frequency_ = 0.5f;
	
}

void GameScene::Update(){
	DebugGUI();

	obj_->SetColor({ 1.0f,0.0f,0.0f,1.0f });

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
	for (auto& wt : WTs_) {
		wt.UpdateMatrix();
	}
	pointLight_.Update();
	spotLight_.Update();
}

void GameScene::DrawBackGround(){

	

}

void GameScene::DrawModel(){

	//obj_->Draw(objWT_, camera_);
	//obj2_->Draw(objWT2_, camera_);
	//terrain_->Draw(terrainWT_, camera_);

	for (size_t index = 0; index < objects_.size(); index++) {
		objects_[index]->Draw(WTs_[index], camera_);
	}

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

	ImGui::Begin("SpotLight");

	ImGui::ColorEdit4("color", &spotLight_.color_.x);
	ImGui::DragFloat3("position", &spotLight_.position_.x, 0.01f);
	ImGui::SliderFloat("intensity", &spotLight_.intensity_, 0.0f, 1.0f);
	ImGui::SliderFloat3("direction", &spotLight_.direction_.x, -1.0f, 1.0f);
	ImGui::SliderFloat("distance", &spotLight_.distance_, 0.0f, 10.0f);
	ImGui::SliderFloat("decay", &spotLight_.decay_, 0.01f, 2.0f);
	ImGui::SliderFloat("cosAngle", &spotLight_.cosAngle_, 0.0f, spotLight_.cosFalloffStart_ - 0.001f);
	ImGui::SliderFloat("cosFalloffStart", &spotLight_.cosFalloffStart_, spotLight_.cosAngle_, 1.0f);

	ImGui::End();

#endif // _DEBUG
}


