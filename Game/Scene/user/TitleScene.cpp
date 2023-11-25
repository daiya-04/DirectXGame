#include "TitleScene.h"

#include "../SceneManager.h"

void TitleScene::Initialize()
{
	
	uint32_t titleTexture = TextureManager::Load("darumaModoshi.png");
	uint32_t pushSpaceTexture = TextureManager::Load("pushSpace.png");

	

	title_.reset(new Sprite(titleTexture, {640.0f,250.0f}, { 800.0f,500.0f },0.0f,{0.5f,0.5f}));
	title_->Initialize();

	pushSpace_.reset(new Sprite(pushSpaceTexture,{640.0f,570.0f}, { 500.0f,100.0f },0.0f,{0.5f,0.5f}));
	pushSpace_->Initialize();
	

}

void TitleScene::Reset()
{
	
	
	title_->Initialize();
	pushSpace_->Initialize();

	
	
}

void TitleScene::Update()
{
	if (input_->TriggerKey(DIK_SPACE)) {
		SceneManager::GetInstace()->ChegeScene(kSELECT);
	}

	

	if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_A)) {

	}

	
	
	


	
	
}

void TitleScene::DrawModel()
{
	

}

void TitleScene::DrawUI()
{
	
	title_->Draw();
	pushSpace_->Draw();
}

TitleScene::~TitleScene() {}

void TitleScene::DebugGUI()
{
#ifdef _DEBUG



#endif // _DEBUG

}
