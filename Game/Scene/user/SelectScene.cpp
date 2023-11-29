#include "SelectScene.h"

#include "../SceneManager.h"
#include "../../../ImGuiManager.h"
#include <cmath>
#include <numbers>

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


	if (input_->TriggerKey(DIK_A) || input_->TriggerKey(DIK_LEFT))
	{
		selectNum_--;
	}
	if (input_->TriggerKey(DIK_D) || input_->TriggerKey(DIK_RIGHT))
	{
		selectNum_++;
	}
	if (input_->TriggerKey(DIK_W) || input_->TriggerKey(DIK_UP))
	{
		if (selectNum_ >= 4) {
			selectNum_ -= 4;
		}
		
	}
	if (input_->TriggerKey(DIK_S) || input_->TriggerKey(DIK_DOWN))
	{
		if (selectNum_ <= 3) {
			selectNum_ += 4;
		}
	}

	if (selectNum_ < 0)
	{
		selectNum_ = 0;
	}
	else if (stageNum_ <= selectNum_)
	{
		selectNum_ = stageNum_ - 1;
	}
	if (input_->TriggerKey(DIK_SPACE)) {
		//SceneManager::GetInstace()->ChegeScene(kGAME);
		SceneManager::GetInstace()->SelectStage(selectNum_);
	}

	for (size_t index = 0; index < stageNum_; index++) {
		number_[index]->SetPosition(stageNumPos_[index]);
		if (selectNum_ == index) {
			number_[index]->SetColor({ 1.0f,0.0f,0.0f,1.0f });
			//1フレームでのパラメータ加算値
			const float step = 1.0f * std::numbers::pi_v<float> / (float)cycle;

			scalingParam_ += step;

			scalingParam_ = std::fmod(scalingParam_, 2.0f * std::numbers::pi_v<float>);

			number_[index]->SetSize({ 96.0f + 32.0f * (std::cosf(scalingParam_) * 0.5f) ,96.0f + 32.0f * (std::cosf(scalingParam_) * 0.5f) });
		}else {
			number_[index]->SetColor({ 1.0f,1.0f,1.0f,1.0f });
			number_[index]->SetSize({ 96.0f,96.0f });
		}
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

void SelectScene::DrawParticle()
{

}


SelectScene::~SelectScene()
{



}

void SelectScene::DebugGUI()
{
#ifdef _DEBUG
	
	ImGui::Begin("SelectScene");

	ImGui::Text("Select : %d", selectNum_);

	ImGui::End();

#endif // _DEBUG
}