#include "ModelScene.h"

#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include "Audio.h"
#include "Input.h"
#include "DirectionalLight.h"
#include "SceneManager.h"
#include <random>

ModelScene::~ModelScene(){}

void ModelScene::Init(){

	camera_.Init();
	pointLight_.Init();
	spotLight_.Init();

	Object3d::SetPointLight(&pointLight_);
	Object3d::SetSpotLight(&spotLight_);

	camera_.translation_ = { 0.0f,7.0f,-16.0f };
	camera_.rotation_ = { 0.5f,0.0f,0.0f };

	uint32_t circle = TextureManager::Load("circle.png");
	uint32_t star = TextureManager::Load("star.png");

	bgmHandle_ = Audio::LoadWave("KIRI.wav");
	
	

	//ステージ情報の読み込み
	levelData_ = std::unique_ptr<LevelData>(LevelLoader::LoadFile("stage"));

	//レベルデータからオブジェクトを生成、配置
	for (auto& objectData : levelData_->objects) {
		std::shared_ptr<Model> model = ModelManager::Load(objectData.fileName);

		WorldTransform wt;
		wt.Init();

		wt.translation_ = objectData.translation;

		if (objectData.objectType == "ground") {
			terrain_.reset(Object3d::Create(model));
			terrainWT_ = wt;
			terrainWT_.rotation_.y = -1.57f;
		}else if (objectData.objectType == "teapot") {
			Object3d* newObject = Object3d::Create(model);

			teapotWTs_.push_back(wt);
			teapots_.push_back(std::unique_ptr<Object3d>(newObject));
		}
	}

	particle1_ = std::make_unique<Particle>();
	particle1_.reset(Particle::Create(circle, 1000));

	emitter1_.translate_ = { 6.0f,3.0f,0.0f };
	emitter1_.count_ = 5;
	emitter1_.frequency_ = 0.5f;

	DirectionalLight::GetInstance()->intensity_ = 0.1f;
	pointLight_.position_ = { 0.0f,3.0f,0.0f };
	pointLight_.color_ = { 1.0f,0.0f,0.0f,1.0f };
	spotLight_.position_ = { 0.0f,1.0f,-7.0f };
	spotLight_.color_ = { 0.0f,0.0f,1.0f,1.0f };
	spotLight_.cosAngle_ = 0.7f;

}

void ModelScene::Update(){
	DebugGUI();

	if (!Audio::GetInstance()->IsValidPlayhandle(playerHandle_)) {
		playerHandle_ = Audio::GetInstance()->SoundPlayWave(bgmHandle_, 0.5f, true);
	}

	std::random_device seedGenerator;
	std::mt19937 randomEngine(seedGenerator());

	if (Input::GetInstance()->TriggerKey(DIK_K)) {
		SceneManager::GetInstance()->ChangeScene("Sprite");
	}

	camera_.CameraControl();

	emitter1_.frequencyTime_ += kDeltaTime;

	if (emitter1_.frequency_ <= emitter1_.frequencyTime_) {
		
		for (size_t count = 0; count < emitter1_.count_; count++) {
			particleData1_.splice(particleData1_.end(), Particle::Emit(emitter1_, randomEngine));
		}
		emitter1_.frequencyTime_ -= emitter1_.frequency_;
	}

	

	for (std::list<Particle::ParticleData>::iterator itParticleData1 = particleData1_.begin(); itParticleData1 != particleData1_.end(); itParticleData1++) {
		itParticleData1->worldTransform_.translation_ += itParticleData1->velocity_ * kDeltaTime;
		itParticleData1->currentTime_ += kDeltaTime;
	}

	//行列更新
	camera_.UpdateViewMatrix();
	for (auto& wt : teapotWTs_) {
		wt.UpdateMatrix();
	}
	terrainWT_.UpdateMatrix();
	pointLight_.Update();
	spotLight_.Update();
}

void ModelScene::DrawBackGround(){



}

void ModelScene::DrawModel(){

	for (size_t index = 0; index < teapots_.size(); index++) {
		teapots_[index]->Draw(teapotWTs_[index], camera_);
	}
	terrain_->Draw(terrainWT_, camera_);

}

void ModelScene::DrawParticleModel(){



}

void ModelScene::DrawParticle(){

	particle1_->Draw(particleData1_, camera_);

}

void ModelScene::DrawUI(){



}

void ModelScene::DebugGUI(){
#ifdef _DEBUG

	ImGui::Begin("camera");

	ImGui::DragFloat3("pos", &camera_.translation_.x, 0.01f);
	ImGui::DragFloat3("rotate", &camera_.rotation_.x, 0.01f);

	ImGui::End();

	ImGui::Begin("Particle");

	ImGui::DragFloat3("emitter pos", &emitter1_.translate_.x, 0.01f);

	ImGui::End();

	ImGui::Begin("object");

	if (ImGui::TreeNode("Teapot 1")) {

		ImGui::DragFloat3("transform", &teapotWTs_[0].translation_.x, 0.01f);
		ImGui::DragFloat3("rotation", &teapotWTs_[0].rotation_.x, 0.01f);
		ImGui::DragFloat3("scale", &teapotWTs_[0].scale_.x, 0.01f);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Teapot 2")) {

		ImGui::DragFloat3("transform", &teapotWTs_[1].translation_.x, 0.01f);
		ImGui::DragFloat3("rotation", &teapotWTs_[1].rotation_.x, 0.01f);
		ImGui::DragFloat3("scale", &teapotWTs_[1].scale_.x, 0.01f);

		ImGui::TreePop();
	}

	ImGui::End();

	ImGui::Begin("Light");

	if (ImGui::TreeNode("PointLight")) {

		ImGui::ColorEdit4("color", &pointLight_.color_.x);
		ImGui::DragFloat3("position", &pointLight_.position_.x, 0.01f);
		ImGui::SliderFloat("intensity", &pointLight_.intensity_, 0.0f, 1.0f);
		ImGui::SliderFloat("radius", &pointLight_.radius_, 0.0f, 10.0f);
		ImGui::SliderFloat("decay", &pointLight_.decay_, 0.01f, 2.0f);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("SpotLight")) {

		ImGui::ColorEdit4("color", &spotLight_.color_.x);
		ImGui::DragFloat3("position", &spotLight_.position_.x, 0.01f);
		ImGui::SliderFloat("intensity", &spotLight_.intensity_, 0.0f, 1.0f);
		ImGui::SliderFloat3("direction", &spotLight_.direction_.x, -1.0f, 1.0f);
		ImGui::SliderFloat("distance", &spotLight_.distance_, 0.0f, 10.0f);
		ImGui::SliderFloat("decay", &spotLight_.decay_, 0.01f, 2.0f);
		ImGui::SliderFloat("cosAngle", &spotLight_.cosAngle_, 0.0f, spotLight_.cosFalloffStart_ - 0.001f);
		ImGui::SliderFloat("cosFalloffStart", &spotLight_.cosFalloffStart_, spotLight_.cosAngle_ + 0.001f, 1.0f);

		ImGui::TreePop();
	}

	ImGui::End();

#endif // _DEBUG
}
