#include "ResultScene.h"

#include"SceneManager.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"

ResultScene::~ResultScene(){

}

void ResultScene::Init() {	

	uint32_t backGroundTex = TextureManager::Load("white.png");
	uint32_t titleTex = TextureManager::Load("title.png");
	uint32_t pressTex = TextureManager::Load("pressA.png");

	background_.reset(new Sprite(backGroundTex, { 640.0f,360.0f }, { 1280.0f,720.0f }, 
		0.0f, { 0.5f,0.5f }, { 0.2f,0.2f,0.2f,1.0f }));

	background_->Initialize();

	titleTrnas_.Init();
	titleTrnas_.translation_ = { 640.0f,230.0f,0.0f };
	titleTrnas_.scale_ = { 820.0f,420.0f,0.0f };

	title_.reset(new Sprite(titleTex, { titleTrnas_.translation_.x,titleTrnas_.translation_.y }, { titleTrnas_.scale_.x,titleTrnas_.scale_.y },
		0.0f, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }));

	title_->Initialize();

	pressTrnas_.Init();
	pressTrnas_.translation_ = { 640.0f,500.0f,0.0f };
	pressTrnas_.scale_ = { 256.0f,64.0f,0.0f };

	press_.reset(new Sprite(pressTex, { pressTrnas_.translation_.x,pressTrnas_.translation_.y }, { 256.0f,64.0f }, 0.0f, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }));

	press_->Initialize();
	input_ = Input::GetInstance();
	
	isNext_ = false;
}

void ResultScene::Update() {
	DebugGUI();

	title_->SetPosition({ titleTrnas_.translation_.x,titleTrnas_.translation_.y });
	title_->SetSize({ titleTrnas_.scale_.x,titleTrnas_.scale_.y });

	press_->SetPosition({ pressTrnas_.translation_.x,pressTrnas_.translation_.y });
	press_->SetSize({ pressTrnas_.scale_.x,pressTrnas_.scale_.y });

	pressTrnas_.UpdateMatrix();
	titleTrnas_.UpdateMatrix();

	if ((input_->TriggerKey(DIK_RETURN) || input_->TriggerButton(XINPUT_GAMEPAD_A)) && !isNext_) {
		isNext_ = true;
		SceneManager::GetInstance()->ChangeScene(AbstractSceneFactory::SceneName::Select);
	}
	
}

void ResultScene::DrawBackGround() {
	background_->Draw();
	//press_->Draw();
	//title_->Draw();
}

void ResultScene::DrawModel() {

}

void ResultScene::DrawParticleModel() {

}

void ResultScene::DrawParticle() {
	
}

void ResultScene::DrawUI() {

}

void ResultScene::DebugGUI() {
#ifdef _DEBUG
	ImGui::Begin("TitleSprite");
	ImGui::DragFloat3("trans", &pressTrnas_.translation_.x, 0.1f);
	ImGui::DragFloat3("scale", &pressTrnas_.scale_.x, 0.1f);
	ImGui::End();
#endif // _DEBUG

}
