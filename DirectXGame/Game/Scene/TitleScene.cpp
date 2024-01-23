#include "TitleScene.h"

#include"SceneManager.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"

TitleScene::~TitleScene(){

}

void TitleScene::Init() {	

	uint32_t backGroundTex = TextureManager::Load("white.png");
	uint32_t titleTex = TextureManager::Load("title.png");

	background_.reset(new Sprite(backGroundTex, { 640.0f,360.0f }, { 1280.0f,720.0f }, 
		0.0f, { 0.5f,0.5f }, { 0.2f,0.2f,0.2f,1.0f }));

	background_->Initialize();

	titleTrnas_.Init();
	titleTrnas_.translation_ = { 640.0f,230.0f,0.0f };
	titleTrnas_.scale_ = { 820.0f,420.0f,0.0f };

	title_.reset(new Sprite(titleTex, { titleTrnas_.translation_.x,titleTrnas_.translation_.y }, { titleTrnas_.scale_.x,titleTrnas_.scale_.y },
		0.0f, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }));

	title_->Initialize();

	input_ = Input::GetInstance();
	
	isNext_ = false;
}

void TitleScene::Update() {
	DebugGUI();

	title_->SetPosition({ titleTrnas_.translation_.x,titleTrnas_.translation_.y });
	title_->SetSize({ titleTrnas_.scale_.x,titleTrnas_.scale_.y });

	titleTrnas_.UpdateMatrix();

	if ((input_->TriggerKey(DIK_RETURN) || input_->TriggerButton(XINPUT_GAMEPAD_A)) && !isNext_) {
		isNext_ = true;
		SceneManager::GetInstance()->ChangeScene(AbstractSceneFactory::SceneName::Select);
	}
	
}

void TitleScene::DrawBackGround() {
	background_->Draw();
	title_->Draw();
}

void TitleScene::DrawModel() {

}

void TitleScene::DrawParticleModel() {

}

void TitleScene::DrawParticle() {
	
}

void TitleScene::DrawUI() {

}

void TitleScene::DebugGUI() {
#ifdef _DEBUG
	ImGui::Begin("TitleSprite");
	ImGui::DragFloat3("trans", &titleTrnas_.translation_.x, 0.1f);
	ImGui::DragFloat3("scale", &titleTrnas_.scale_.x, 0.1f);
	ImGui::End();
#endif // _DEBUG

}
