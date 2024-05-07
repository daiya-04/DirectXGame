#include "TitleScene.h"

#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include "SceneManager.h"
#include "Input.h"

TitleScene::~TitleScene() {}

void TitleScene::Init(){

	uint32_t titleTex = TextureManager::Load("title.png");
	uint32_t buttonTex = TextureManager::Load("AButton.png");


	title_.reset(Sprite::Create(titleTex, { 640.0f,360.0f }));

	Abutton_.reset(Sprite::Create(buttonTex, {640.0f,520.0f}));

}

void TitleScene::Update(){

#ifdef _DEBUG

	if (Input::GetInstance()->PushKey(DIK_2)) {
		SceneManager::GetInstance()->ChangeScene("Game");
	}

#endif // _DEBUG

	if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_A)) {
		SceneManager::GetInstance()->ChangeScene("Game");
	}

	if (++count_ <= 60) {
		Abutton_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
	}
	else {
		Abutton_->SetColor({ 1.0f,1.0f,1.0f,0.0f });
	}

	if (count_ >= 90) {
		count_ = 0;
	}

}

void TitleScene::DrawBackGround(){

	title_->Draw();
	Abutton_->Draw();

}

void TitleScene::DrawModel(){



}

void TitleScene::DrawParticleModel(){



}

void TitleScene::DrawParticle(){



}

void TitleScene::DrawUI(){



}

void TitleScene::DrawPostEffect() {



}

void TitleScene::DebugGUI(){
#ifdef _DEBUG




#endif // _DEBUG
}


