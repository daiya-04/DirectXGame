#include "TitleScene.h"

#include"SceneManager.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include"Ease/Ease.h"
#include"SelectScene.h"

TitleScene::~TitleScene(){

}

void TitleScene::Init() {	

	uint32_t backGroundTex = TextureManager::Load("white.png");
	uint32_t titleTex = TextureManager::Load("title.png");
	uint32_t pressTex = TextureManager::Load("PressAButton.png");

	background_.reset(new Sprite(backGroundTex, { 640.0f,360.0f }, { 1280.0f,720.0f }, 
		0.0f, { 0.5f,0.5f }, { 0.2f,0.2f,0.2f,1.0f }));

	background_->Initialize();

	titleTrnas_.Init();
	titleTrnas_.translation_ = { 640.0f,230.0f,0.0f };
	titleTrnas_.scale_ = { 576.0f,192.0f,0.0f };

	title_.reset(new Sprite(titleTex, { titleTrnas_.translation_.x,titleTrnas_.translation_.y }, { titleTrnas_.scale_.x,titleTrnas_.scale_.y },
		0.0f, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }));

	title_->Initialize();

	pressTrnas_.Init();
	pressTrnas_.translation_ = { 640.0f,500.0f,0.0f };
	pressTrnas_.scale_ = { 508.0f,72.0f,0.0f };

	press_.reset(new Sprite(pressTex, { pressTrnas_.translation_.x,pressTrnas_.translation_.y }, { 256.0f,64.0f }, 0.0f, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }));

	press_->Initialize();

	i = 0.0f;

	alpha = 1.0f;

	input_ = Input::GetInstance();
	
	isNext_ = false;
}

void TitleScene::Update() {
	DebugGUI();

	title_->SetPosition({ titleTrnas_.translation_.x,titleTrnas_.translation_.y });
	title_->SetSize({ titleTrnas_.scale_.x,titleTrnas_.scale_.y });

	press_->SetPosition({ pressTrnas_.translation_.x,pressTrnas_.translation_.y });
	press_->SetSize({ pressTrnas_.scale_.x,pressTrnas_.scale_.y });


	if (i < 0.0f) {
		magnification *= -1;
	}
	else if (i > 1.0f) {
		magnification *= -1;
	}
	
	i += (addSpeed_ * magnification);
	alpha = Ease::Easing(Ease::EaseName::EaseOutSine, 0.0f, 1.0f, i);

	press_->SetColor({ 1.0f,1.0f,1.0f,alpha });

	pressTrnas_.UpdateMatrix();
	titleTrnas_.UpdateMatrix();

	if ((input_->TriggerKey(DIK_RETURN) || input_->TriggerButton(XINPUT_GAMEPAD_A)) && !isNext_) {
		isNext_ = true;
		
		SceneManager::GetInstance()->ChangeScene(AbstractSceneFactory::SceneName::Select);
	}
	
}

void TitleScene::DrawBackGround() {
	background_->Draw();
	press_->Draw();
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
	ImGui::DragFloat3("titletrans", &titleTrnas_.translation_.x, 0.1f);
	ImGui::DragFloat3("titlescale", &titleTrnas_.scale_.x, 0.1f);
	ImGui::DragFloat3("presstrans", &pressTrnas_.translation_.x, 0.1f);
	ImGui::DragFloat3("pressscale", &pressTrnas_.scale_.x, 0.1f);
	ImGui::DragFloat("i", &i, 0.01f);
	ImGui::End();
#endif // _DEBUG

}
