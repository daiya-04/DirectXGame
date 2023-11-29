#include "TitleScene.h"

#include "../SceneManager.h"

void TitleScene::Initialize()
{

    uint32_t titleTexture = TextureManager::Load("darumaModoshi.png");
    uint32_t pushSpaceTexture = TextureManager::Load("pushSpace.png");



    title_.reset(new Sprite(titleTexture, { 640.0f,300.0f }, { 800.0f,500.0f }, 0.0f, { 0.5f,0.5f }));
    title_->Initialize();

    pushSpace_.reset(new Sprite(pushSpaceTexture, { 640.0f,600.0f }, { 500.0f,100.0f }, 0.0f, { 0.5f,0.5f }));
    pushSpace_->Initialize();

    pushSpaceScaling_.reset(new Sprite(pushSpaceTexture, { 640.0f,600.0f }, { 500.0f,100.0f }, 0.0f, { 0.5f,0.5f }));
    pushSpaceScaling_->Initialize();

    size_t bgmHandle = Audio::GetInstance()->SoundLoadWave("outGameBGM.wav");
    playHandle_ = Audio::GetInstance()->SoundPlayLoopStart(bgmHandle);
    Audio::GetInstance()->SetValume(playHandle_, 0.2f);

}

void TitleScene::Reset()
{
    title_->Initialize();
    pushSpace_->Initialize();
    pushSpaceScaling_->Initialize();

    Audio::GetInstance()->SoundPlayLoopEnd(playHandle_);
    size_t bgmHandle = Audio::GetInstance()->SoundLoadWave("outGameBGM.wav");
    playHandle_ = Audio::GetInstance()->SoundPlayLoopStart(bgmHandle);
    Audio::GetInstance()->SetValume(playHandle_, 0.2f);

}

void TitleScene::Update()
{
    if (input_->TriggerKey(DIK_SPACE)) {
        SceneManager::GetInstace()->ChegeScene(kSELECT);
    }



    if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_A)) {

    }

    // 拡縮透明
    auto easeOutCirc = [](float t) { return std::sqrtf(1 - std::powf(t - 1, 2)); };

    pushSpaceScaling_->SetSize(Lerp(easeOutCirc(scaleT), pushSpace_.get()->GetSize(), pushSpace_.get()->GetSize() * 1.2f));
    pushSpaceScaling_->SetColor({ 1.0f,1.0f,1.0f, Lerp(easeOutCirc(scaleT), 0.8f, 0.0f) });

    scaleT += 0.02f;
    if (scaleT > 0.9f) {
        scaleT = 0.0f;
    }


}

void TitleScene::DrawModel()
{


}

void TitleScene::DrawUI()
{

    pushSpaceScaling_->Draw();
    title_->Draw();
    pushSpace_->Draw();
}

void TitleScene::DrawParticle()
{

}

TitleScene::~TitleScene() {}

void TitleScene::DebugGUI()
{
#ifdef _DEBUG



#endif // _DEBUG

}
