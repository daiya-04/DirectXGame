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
    uint32_t stageImgFrameFaceTexture = TextureManager::Load("imageFrameFace.png");
    uint32_t stageImageTexture = TextureManager::Load("imageStage.png");

    stageNumPos_ = {
        {
            {100.0f,300.0f},{250.0f,300.0f},{400.0f,300.0f},{550.0f,300.0f},
            {100.0f,500.0f},{250.0f,500.0f},{400.0f,500.0f},{550.0f,500.0f}
        }
    };

    for (size_t index = 0; index < stageNum_; index++) {
        number_[index].reset(new Sprite(numberTexture, { stageNumPos_[index] }, { 96.0f,96.0f }));
        number_[index]->Initialize();
        number_[index]->SetTextureArea({ 64.0f + (64.0f * index),0.0f }, { 64.0f,64.0f });
        number_[index]->SetAnchorpoint({ 0.5f,0.5f });

        stageImages_[index].reset(new Sprite(stageImageTexture, { 940.0f, 400.0f }, { 480.0f, 270.0f }));
        stageImages_[index]->Initialize();
        stageImages_[index]->SetTextureArea({ 640.0f + (640.0f * index), 0.0f }, { 640.0f, 360.0f });
        stageImages_[index]->SetAnchorpoint({ 0.5f, 0.5f });
    }

    header_.reset(new Sprite(headerTexture, { 640.0f,100.0f }, { 800.0f,100.0f }, 0.0f, { 0.5f,0.5f }));
    header_->Initialize();

    stageImgFrame_.reset(new Sprite(stageImgFrameTexture, { 940.0f,400.0f }, { 680.0f,400.0f }, 0.0f, { 0.5f,0.5f }));
    stageImgFrame_->Initialize();
    stageImgFrameFace_.reset(new Sprite(stageImgFrameFaceTexture, { 940.0f,400.0f }, { 680.0f,400.0f }, 0.0f, { 0.5f,0.5f }));
    stageImgFrameFace_->Initialize();
}

void SelectScene::Reset()
{
    for (size_t index = 0; index < stageNum_; index++) {
        number_[index]->Initialize();
        number_[index]->SetTextureArea({ 64.0f + (64.0f * index),0.0f }, { 64.0f,64.0f });
        number_[index]->SetAnchorpoint({ 0.5f,0.5f });

        stageImages_[index]->Initialize();
        stageImages_[index]->SetTextureArea({ 640.0f + (640.0f * index), 0.0f }, { 640.0f, 360.0f });
        stageImages_[index]->SetAnchorpoint({ 0.5f, 0.5f });
    }
    header_->Initialize();
    stageImgFrame_->Initialize();
    stageImgFrameFace_->Initialize();


}

void SelectScene::Update()
{
    DebugGUI();
    size_t selectSEHandle = Audio::GetInstance()->SoundLoadWave("select.wav");

    if (input_->TriggerKey(DIK_A) || input_->TriggerKey(DIK_LEFT))
    {
        size_t selectSEPlayerHandle = Audio::GetInstance()->SoundPlayWave(selectSEHandle);
        selectNum_--;
    }
    if (input_->TriggerKey(DIK_D) || input_->TriggerKey(DIK_RIGHT))
    {
        size_t selectSEPlayerHandle = Audio::GetInstance()->SoundPlayWave(selectSEHandle);
        selectNum_++;
    }
    if (input_->TriggerKey(DIK_W) || input_->TriggerKey(DIK_UP))
    {
        size_t selectSEPlayerHandle = Audio::GetInstance()->SoundPlayWave(selectSEHandle);
        if (selectNum_ >= 4) {
            selectNum_ -= 4;
        }

    }
    if (input_->TriggerKey(DIK_S) || input_->TriggerKey(DIK_DOWN))
    {
        size_t selectSEPlayerHandle = Audio::GetInstance()->SoundPlayWave(selectSEHandle);
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
        size_t selectSEHandle = Audio::GetInstance()->SoundLoadWave("select.wav");
        size_t selectSEPlayerHandle = Audio::GetInstance()->SoundPlayWave(selectSEHandle);
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
        }
        else {
            number_[index]->SetColor({ 1.0f,1.0f,1.0f,1.0f });
            number_[index]->SetSize({ 96.0f,96.0f });
        }
    }

    stageImgFrameFace_->SetPosition({ 940.0f + 10.0f * std::sinf(scalingParam_),400.0f + 10.0f * std::sinf(scalingParam_ * 2.0f) });
}

void SelectScene::DrawModel()
{



}

void SelectScene::DrawUI()
{

    stageImgFrame_->Draw();
    for (size_t index = 0; index < stageNum_; index++) {
        number_[index]->Draw();
        if (selectNum_ == index) {
            stageImages_[index]->Draw();
        }
    }
    stageImgFrameFace_->Draw();
    header_->Draw();

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