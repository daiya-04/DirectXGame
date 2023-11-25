#include "SelectScene.h"

#include "../SceneManager.h"
#include "../../../ImGuiManager.h"

//const size_t SelectScene::stageNum_ = 8;

void SelectScene::Initialize()
{
	
	uint32_t numberTexture = TextureManager::Load("number.png");
	uint32_t headerTexture = TextureManager::Load("dokoDaruma.png");
	uint32_t stageImgFrameTexture = TextureManager::Load("imageFrame.png");

	stageNumPos_ = {
		{
			{100.0f,300.0f},{250.0f,300.0f},{400.0f,300.0f},{550.0f,300.0f},
			{100.0f,500.0f},{250.0f,500.0f},{400.0f,500.0f},{550.0f,500.0f}
		}
	};

	for (size_t index = 0; index < stageNum_; index++) {
		number_[index].reset(new Sprite(numberTexture, {stageNumPos_[index]}, {96.0f,96.0f}));
		number_[index]->Initialize();
		number_[index]->SetTextureArea({ 32.0f + (32.0f * index),0.0f }, { 32.0f,32.0f });
		number_[index]->SetAnchorpoint({ 0.5f,0.5f });
	}

	header_.reset(new Sprite(headerTexture, { 640.0f,100.0f }, { 800.0f,100.0f }, 0.0f, { 0.5f,0.5f }));
	header_->Initialize();

	stageImgFrame_.reset(new Sprite(stageImgFrameTexture, {940.0f,400.0f}, { 680.0f,400.0f }, 0.0f, { 0.5f,0.5f }));
	stageImgFrame_->Initialize();

}

void SelectScene::Reset()
{
	for (size_t index = 0; index < stageNum_; index++) {
		number_[index]->Initialize();
		number_[index]->SetTextureArea({ 32.0f + (32.0f * index),0.0f }, { 32.0f,32.0f });
		number_[index]->SetAnchorpoint({ 0.5f,0.5f });
	}
	header_->Initialize();
	stageImgFrame_->Initialize();


}

void SelectScene::Update()
{
	DebugGUI();

	if (input_->TriggerKey(DIK_SPACE)) {
		SceneManager::GetInstace()->ChegeScene(kGAME);
	}

	for (size_t index = 0; index < stageNum_; index++) {
		number_[index]->SetPosition(stageNumPos_[index]);
	}

}

void SelectScene::DrawModel()
{



}

void SelectScene::DrawUI()
{
	
	for (size_t index = 0; index < stageNum_; index++) {
		number_[index]->Draw();
	}
	header_->Draw();
	stageImgFrame_->Draw();

}


SelectScene::~SelectScene()
{



}

void SelectScene::DebugGUI()
{
#ifdef _DEBUG
	


#endif // _DEBUG
}