#include "TitleScene.h"

#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include "SceneManager.h"
#include "Input.h"

TitleScene::~TitleScene() {}

void TitleScene::Init(){

	uint32_t titleTex = TextureManager::Load("title.png");
	uint32_t buttonTex = TextureManager::Load("button.png");


	title_.reset(new Sprite(titleTex, { 640.0f,360.0f }, { 1280.0f,720.0f }));
	title_->Initialize();

	button_.reset(new Sprite(buttonTex, {}, {}));
	button_->Initialize();

}

void TitleScene::Update(){

#ifdef _DEBUG

	if (Input::GetInstance()->PushKey(DIK_2)) {
		SceneManager::GetInstance()->ChangeScene("Game");
	}

#endif // _DEBUG

	if (Input::GetInstance()->PushKey(XINPUT_GAMEPAD_A)) {
		SceneManager::GetInstance()->ChangeScene("Game");
	}

}

void TitleScene::DrawBackGround(){

	title_->Draw();
	button_->Draw();

}

void TitleScene::DrawModel(){



}

void TitleScene::DrawParticleModel(){



}

void TitleScene::DrawParticle(){



}

void TitleScene::DrawUI(){



}

void TitleScene::DebugGUI(){
#ifdef _DEBUG




#endif // _DEBUG
}


