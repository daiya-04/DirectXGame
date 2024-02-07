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

	audio_ = Audio::GetInstance();

	titleBGM = Audio::LoadWave("Title2.wav");
	gameBGM = Audio::LoadWave("Game1.wav");
	clearBGM = Audio::LoadWave("Result3.wav");

	audio_->SoundPlayWave(titleBGM, 0.3f, true);

	spotLight_.Init();
	spotLight_.intensity_ = 0.0f;
	pointLight_.Init();
	spotLight_.intensity_ = 0.0f;
	pointLight_.Update();
	spotLight_.Update();

	camera_.Init();

	Object3d::SetPointLight(&pointLight_);
	Object3d::SetSpotLight(&spotLight_);

	uint32_t backGroundTex = TextureManager::Load("white.png");

	fade_.reset(new Sprite(backGroundTex, { 640.0f,360.0f }, { 1280.0f,720.0f },
		0.0f, { 0.5f,0.5f }, { 0.0f,0.0f,0.0f,fadeAlpha_ }));

	fade_->Initialize();

	/*uint32_t circle = TextureManager::Load("circle.png");
	particle_ = std::make_unique<Particle>();
	particle_.reset(Particle::Create(circle, 800));

	emitter_.count_ = 15;
	emitter_.frequency_ = 0.05f;
	emitter_.translate_ = { 0.0f,-8.0f,0.0f };


	accelerationField_.acceleration_ = { 0.0f,8.0f,0.0f };
	accelerationField_.area_.min = { -100.0f,-100.0f,-100.0f };
	accelerationField_.area_.max = { 100.0f,100.0f,100.0f };

	isField_ = false;*/

}

void SceneManager::Update(){
	if (nextScene_ && fadeAlpha_ < 1.0f) {
		fadeAlpha_ += 0.02f;
	}

	if (fadeAlpha_ >= 1.0f && nextScene_) {
		nextSceneInit_ = true;
	}

	if (nextSceneInit_) {
		if (nowSceneName_ == AbstractSceneFactory::SceneName::Select && nextSceneName_ == AbstractSceneFactory::SceneName::Game) {
			audio_->SoundPlayLoopEnd(titleBGM);
			gameBGM= audio_->SoundPlayWave(gameBGM, 0.3f, true);
			titleBGM = Audio::LoadWave("Title2.wav");
		}
		else if (nowSceneName_ == AbstractSceneFactory::SceneName::Game && nextSceneName_ == AbstractSceneFactory::SceneName::Result) {
			audio_->SoundPlayLoopEnd(gameBGM);			
			clearBGM= audio_->SoundPlayWave(clearBGM, 0.3f, true);
			gameBGM = Audio::LoadWave("Game1.wav");
		}
		else if (nowSceneName_ == AbstractSceneFactory::SceneName::Result && nextSceneName_ == AbstractSceneFactory::SceneName::Select) {
			audio_->SoundPlayLoopEnd(clearBGM);			
			titleBGM= audio_->SoundPlayWave(titleBGM, 0.3f, true);
			clearBGM = Audio::LoadWave("Result3.wav");
		}
		else if (nowSceneName_ == AbstractSceneFactory::SceneName::Game && nextSceneName_ == AbstractSceneFactory::SceneName::Select) {
			audio_->SoundPlayLoopEnd(gameBGM);
			titleBGM = audio_->SoundPlayWave(titleBGM, 0.3f, true);
			gameBGM = Audio::LoadWave("Game1.wav");
		}
		scene_ = std::move(nextScene_);
		nowSceneName_ = nextSceneName_;
		nextSceneName_ = AbstractSceneFactory::SceneName::NONE;
		scene_->Init();
		nextSceneInit_ = false;
		
	}

	fade_->SetColor({ 0.0f,0.0f,0.0f,fadeAlpha_ });

#ifdef _DEBUG
	ImGui::Begin("Title");
	/*ImGui::DragInt("particleNum", &particleNum_);
	ImGui::Checkbox("FieldEffect", &isField_);
	ImGui::DragInt("count", reinterpret_cast<int*>(&emitter_.count_), 1.0f, 1, 15);
	ImGui::DragFloat("frequency", &emitter_.frequency_, 0.01f, 0.01f, 1.0f);
	ImGui::DragFloat3("transform", &emitter_.translate_.x, 0.1f);*/
	ImGui::DragFloat("Alpha", &fadeAlpha_, 0.1f);
	if (ImGui::Button("Go NextScene")) {
		ChangeScene(AbstractSceneFactory::SceneName::Select);
	}
	ImGui::End();
#endif // _DEBUG
	if (!nextScene_ && fadeAlpha_ > 0.0f) {
		fadeAlpha_ -= 0.02f;
	}

	
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
	
	scene_->DrawParticle();

	Particle::postDraw();

	///UI
	Sprite::preDraw(commandList);

	scene_->DrawUI();
	fade_->Draw();
	Sprite::postDraw();

}

void SceneManager::ChangeScene(AbstractSceneFactory::SceneName sceneName){

	assert(nextScene_ == nullptr);

	nextSceneName_ = sceneName;
	nextScene_ = sceneFactory_->CreateScene(sceneName);
}


