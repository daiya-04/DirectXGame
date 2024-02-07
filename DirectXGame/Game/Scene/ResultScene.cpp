#include "ResultScene.h"
#include"GameScene.h"
#include"SceneManager.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"

ResultScene::~ResultScene(){

}

void ResultScene::Init() {	

	uint32_t backGroundTex = TextureManager::Load("white.png");
	uint32_t titleTex = TextureManager::Load("clear.png");
	uint32_t pressTex = TextureManager::Load("goSelect.png");

	background_.reset(new Sprite(backGroundTex, { 640.0f,360.0f }, { 1280.0f,720.0f }, 
		0.0f, { 0.5f,0.5f }, { 0.2f,0.2f,0.2f,1.0f }));

	background_->Initialize();

	titleTrnas_.Init();
	titleTrnas_.translation_ = { 640.0f,150.0f,0.0f };
	titleTrnas_.scale_ = {512.0f,512.0f,0.0f };

	title_.reset(new Sprite(titleTex, { titleTrnas_.translation_.x,titleTrnas_.translation_.y }, { titleTrnas_.scale_.x,titleTrnas_.scale_.y },
		0.0f, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }));

	title_->Initialize();

	pressTrnas_.Init();
	pressTrnas_.translation_ = { 720.0f,600.0f,0.0f };
	pressTrnas_.scale_ = { 508.0f,72.0f,0.0f };

	press_.reset(new Sprite(pressTex, { pressTrnas_.translation_.x,pressTrnas_.translation_.y }, { 256.0f,64.0f }, 0.0f, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }));

	press_->Initialize();

	timeCounter_ = std::make_unique<TimeCounter>();
	timeCounter_->Init();
	timenum_ = GameScene::GetTimeNum();
	

	pos_ = { 640.0f,400.0f };
	scale_ = { 128.0f,128.0f };

	input_ = Input::GetInstance();
	
	SEHandle_ = Audio::LoadWave("title.wav");

	isNext_ = false;
}

void ResultScene::Update() {
	DebugGUI();

	title_->SetPosition({ titleTrnas_.translation_.x,titleTrnas_.translation_.y });
	title_->SetSize({ titleTrnas_.scale_.x,titleTrnas_.scale_.y });

	press_->SetPosition({ pressTrnas_.translation_.x,pressTrnas_.translation_.y });
	press_->SetSize({ pressTrnas_.scale_.x,pressTrnas_.scale_.y });

	timeCounter_->SetPosition(pos_);
	timeCounter_->SetScale(scale_);

	pressTrnas_.UpdateMatrix();
	titleTrnas_.UpdateMatrix();

	timeCounter_->SetNumberCount(timenum_);

	if ((input_->TriggerKey(DIK_RETURN) || input_->TriggerButton(XINPUT_GAMEPAD_A)) && !isNext_) {
		isNext_ = true;
		Audio::GetInstance()->SoundPlayWave(SEHandle_, 0.5f, false);
		SceneManager::GetInstance()->ChangeScene(AbstractSceneFactory::SceneName::Select);
	}
	
}

void ResultScene::DrawBackGround() {
	background_->Draw();
	press_->Draw();
	title_->Draw();
}

void ResultScene::DrawModel() {

}

void ResultScene::DrawParticleModel() {

}

void ResultScene::DrawParticle() {
	
}

void ResultScene::DrawUI() {
	timeCounter_->Draw();
}

void ResultScene::DebugGUI() {
#ifdef _DEBUG
	ImGui::Begin("TitleSprite");
	ImGui::DragFloat3("trans", &pressTrnas_.translation_.x, 0.1f);
	ImGui::DragFloat3("scale", &pressTrnas_.scale_.x, 0.1f);
	ImGui::DragFloat2("timePos", &pos_.x, 1.0f);
	ImGui::DragFloat2("timeScale", &scale_.x, 0.1f, 1.0f, 256.0f);
	ImGui::End();
#endif // _DEBUG

}
