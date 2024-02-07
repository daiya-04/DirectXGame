#include "ResultScene.h"
#include"GameScene.h"
#include"SelectScene.h"
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
	uint32_t timeSTex = TextureManager::Load("timeS.png");
	uint32_t bestUpdateTex = TextureManager::Load("bestUpdate.png");

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

	timeSTrnas_.Init();
	timeSTrnas_.translation_ = { 848.0f,400.0f,0.0f };
	timeSTrnas_.scale_ = { 128.0f,128.0f,0.0f };

	timeS_.reset(new Sprite(timeSTex, { timeSTrnas_.translation_.x,timeSTrnas_.translation_.y }, { 128.0f,128.0f }, 0.0f, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }));

	timeS_->Initialize();

	bestUpdateWT_.Init();
	bestUpdateWT_.translation_ = { 640.0f,270.0f,0.0f };
	bestUpdateWT_.scale_ = { 384.0f,108.0f,0.0f };

	bestUpdate_.reset(new Sprite(bestUpdateTex, { bestUpdateWT_.translation_.x,bestUpdateWT_.translation_.y }, { bestUpdateWT_.scale_.x,bestUpdateWT_.scale_.y }, 0.0f, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }));

	bestUpdate_->Initialize();

	timeCounter_ = std::make_unique<TimeCounter>();
	timeCounter_->Init();
	timenum_ = GameScene::GetTimeNum();
	

	pos_ = { 640.0f,400.0f };
	scale_ = { 128.0f,128.0f };

	input_ = Input::GetInstance();
	
	SEHandle_ = Audio::LoadWave("title.wav");

	isNext_ = false;

	isBest_ = false;
}

void ResultScene::Update() {
	DebugGUI();

	title_->SetPosition({ titleTrnas_.translation_.x,titleTrnas_.translation_.y });
	title_->SetSize({ titleTrnas_.scale_.x,titleTrnas_.scale_.y });

	press_->SetPosition({ pressTrnas_.translation_.x,pressTrnas_.translation_.y });
	press_->SetSize({ pressTrnas_.scale_.x,pressTrnas_.scale_.y });

	timeS_->SetPosition({ timeSTrnas_.translation_.x,timeSTrnas_.translation_.y });
	timeS_->SetSize({ timeSTrnas_.scale_.x,timeSTrnas_.scale_.y });

	bestUpdate_->SetPosition({ bestUpdateWT_.translation_.x,bestUpdateWT_.translation_.y });
	bestUpdate_->SetSize({ bestUpdateWT_.scale_.x,bestUpdateWT_.scale_.y });

	timeCounter_->SetPosition(pos_);
	timeCounter_->SetScale(scale_);

	pressTrnas_.UpdateMatrix();
	titleTrnas_.UpdateMatrix();

	if (timenum_ < SelectScene::GetBestTime(SelectScene::GetSelectNumber())) {
		isBest_ = true;
		SelectScene::SetBestTime(SelectScene::GetSelectNumber(), timenum_);
	}
	else if (SelectScene::GetBestTime(SelectScene::GetSelectNumber()) == 0.0f){
		isBest_ = true;
		SelectScene::SetBestTime(SelectScene::GetSelectNumber(), timenum_);
	}
	

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
	timeS_->Draw();
	if (isBest_){
		bestUpdate_->Draw();
	}
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
