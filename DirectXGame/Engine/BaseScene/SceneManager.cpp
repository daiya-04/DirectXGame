#include "SceneManager.h"
#include "SceneFactory.h"
#include "DirectXCommon.h"
#include "Sprite.h"
#include "Object3d.h"
#include "Particle.h"
#include <cassert>
#include <TextureManager.h>
#include"ImGuiManager.h"

SceneManager* SceneManager::GetInstance(){
	static SceneManager instance;

	return &instance;
}

void SceneManager::Init() {

	sceneFactory_ = std::make_unique<SceneFactory>();

	

	camera_.Init();

	uint32_t circle = TextureManager::Load("circle.png");
	particle_ = std::make_unique<Particle>();
	particle_.reset(Particle::Create(circle, 600));

	emitter_.count_ = 15;
	emitter_.frequency_ = 0.07f;
	emitter_.translate_ = { 0.0f,-8.0f,0.0f };


	accelerationField_.acceleration_ = { 0.0f,8.0f,0.0f };
	accelerationField_.area_.min = { -100.0f,-100.0f,-100.0f };
	accelerationField_.area_.max = { 100.0f,100.0f,100.0f };

	isField_ = false;

}

void SceneManager::Update(){
	if (nextSceneName_ == chackSelectName_ || nowSceneName_ == chackSelectName_) {
		std::random_device seedGenerator;
		std::mt19937 randomEngine(seedGenerator());

		particleNum_ = particle_->GetParticleNum();

		for (std::list<Particle::ParticleData>::iterator itParticle = particles_.begin(); itParticle != particles_.end(); itParticle++) {
			if (true) {
				if (IsCollision(accelerationField_.area_, (*itParticle).worldTransform_.translation_)) {
					(*itParticle).velocity_ += accelerationField_.acceleration_ * kDeltaTime;
				}
			}
			(*itParticle).worldTransform_.translation_ += (*itParticle).velocity_ * kDeltaTime;
			(*itParticle).currentTime_ += kDeltaTime;
		}

		if (isField_) {

			emitter_.frequencyTime_ += kDeltaTime;
			if (emitter_.frequency_ <= emitter_.frequencyTime_) {
				particles_.splice(particles_.end(), Particle::Emit(emitter_, randomEngine));		
	
				emitter_.frequencyTime_ -= emitter_.frequency_;
			}
		}
	}
	if (nextSceneName_ == chackSelectName_) {
		isField_ = true;
	}
	else {
		isField_ = false;
	}
	

	if (nextScene_ && nextSceneName_ == chackSelectName_ &&particleNum_ > 599) {
		scene_ = std::move(nextScene_);
		nowSceneName_ = std::move(nextSceneName_);
		nextSceneName_ = AbstractSceneFactory::SceneName::NONE;
		scene_->Init();
	}
	else if(nextScene_&& nextSceneName_ != chackSelectName_){
		scene_ = std::move(nextScene_);
		nowSceneName_ = std::move(nextSceneName_);
		nextSceneName_ = AbstractSceneFactory::SceneName::NONE;
		scene_->Init();
	}

#ifdef _DEBUG
	ImGui::Begin("Title");
	ImGui::DragInt("particleNum", &particleNum_);
	ImGui::Checkbox("FieldEffect", &isField_);
	ImGui::DragInt("count", reinterpret_cast<int*>(&emitter_.count_), 1.0f, 1, 15);
	ImGui::DragFloat("frequency", &emitter_.frequency_, 0.01f, 0.01f, 1.0f);
	ImGui::DragFloat3("transform", &emitter_.translate_.x, 0.1f);
	if (ImGui::Button("Go NextScene")) {
		ChangeScene(AbstractSceneFactory::SceneName::Select);
	}
	ImGui::End();
#endif // _DEBUG


	scene_->Update();
	camera_.UpdateViewMatrix();
}

void SceneManager::Draw(ID3D12GraphicsCommandList* commandList){

	///背景スプライト
	Sprite::preDraw(commandList);

	scene_->DrawBackGround();

	Sprite::postDraw();

	DirectXCommon::GetInstance()->ClearDepthBaffer();


	///3dオブジェクト
	Object3d::preDraw();

	scene_->DrawModel();

	Object3d::postDraw();

	///パーティクル
	Particle::preDraw();
	particle_->Draw(particles_, camera_);
	scene_->DrawParticle();

	Particle::postDraw();

	///UI
	Sprite::preDraw(commandList);

	scene_->DrawUI();

	Sprite::postDraw();

}

void SceneManager::ChangeScene(AbstractSceneFactory::SceneName sceneName){

	assert(nextScene_ == nullptr);

	nextSceneName_ = sceneName;
	nextScene_ = sceneFactory_->CreateScene(sceneName);
}


