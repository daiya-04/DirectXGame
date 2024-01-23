#include "TitleScene.h"

#include"SceneManager.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"

TitleScene::~TitleScene(){

}

void TitleScene::Init() {	

	uint32_t backGroundTex = TextureManager::Load("white.png");

	background_.reset(new Sprite(backGroundTex, { 640.0f,360.0f }, { 1280.0f,720.0f }, 
		0.0f, { 0.5f,0.5f }, { 0.2f,0.2f,0.2f,1.0f }));

	background_->Initialize();

	input_ = Input::GetInstance();
	
}

void TitleScene::Update() {
	DebugGUI();

	

	if ((input_->TriggerKey(DIK_RETURN) || input_->TriggerButton(XINPUT_GAMEPAD_A))) {
		SceneManager::GetInstance()->ChangeScene(AbstractSceneFactory::SceneName::Select);
	}
	
}

void TitleScene::DrawBackGround() {
	background_->Draw();
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

	

#endif // _DEBUG

}
