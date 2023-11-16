#include "TitleScene.h"

#include "../SceneManager.h"

void TitleScene::Initialize()
{
	uint32_t texture = TextureManager::Load("uvChecker.png");
	sprite_.reset(new Sprite(texture, { 50.0f,50.0f }, { 100.0f,100.0f }));
	sprite_->Initialize();
	sprite_->SetAnchorpoint({ 0.5f,0.5f });

	rotate_ = sprite_->GetRotate();
	pos_ = sprite_->GetPosition();

	pot_.reset(Object3d::Create("teapot"));
	plane_.reset(Object3d::Create("Plane"));

	viewProjection_.reset(new ViewProjection());
	viewProjection_->Initialize();

	wtPlane_.parent_ = &wt_;

}

void TitleScene::Reset()
{
	//uint32_t texture = TextureManager::Load("uvChecker.png");
	//sprite_.reset(new Sprite(texture, { 50.0f,50.0f }, { 100.0f,100.0f }));
	sprite_->Initialize();
	sprite_->SetAnchorpoint({ 0.5f,0.5f });

	rotate_ = sprite_->GetRotate();
	pos_ = sprite_->GetPosition();

	//pot_.reset(Object3d::Create("teapot"));
	//plane_.reset(Object3d::Create("Plane"));

	//viewProjection_.reset(new ViewProjection());
	viewProjection_->Initialize();

	wtPlane_.parent_ = &wt_;
}

void TitleScene::Update()
{
	if (input_->TriggerKey(DIK_SPACE)) {
		SceneManager::GetInstace()->ChegeScene(kGAME);
	}

	XINPUT_STATE joyState{};

	if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_A)) {

	}

	rotate_ += 0.02f;
	pos_.x += 1.0f;
	pos_.y += 1.0f;
	wt_.translation_.x += 0.01f;


	sprite_->SetRotate(rotate_);
	sprite_->SetPosition(pos_);


	wt_.UpdateMatrix();
	wtPlane_.UpdateMatrix();

}

void TitleScene::DrawModel()
{
	pot_->Draw(wt_, *viewProjection_.get());
	plane_->Draw(wtPlane_, *viewProjection_.get());

}

void TitleScene::DrawUI()
{
	sprite_->Draw();
}

TitleScene::~TitleScene() {}

void TitleScene::DebugGUI()
{
#ifdef _DEBUG



#endif // _DEBUG

}
