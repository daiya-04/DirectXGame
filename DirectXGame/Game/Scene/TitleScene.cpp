#include "TitleScene.h"

#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include "SceneManager.h"
#include "Input.h"
#include "GlobalVariables.h"
#include "WinApp.h"
#include "ParticleManager.h"
#include "Easing.h"

TitleScene::~TitleScene() {}

void TitleScene::SetGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	std::string groupName = "TitleText";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Translation", titleText_->worldTransform_.translation_);
	globalVariables->AddItem(groupName, "Rotation", titleText_->worldTransform_.rotation_);
	globalVariables->AddItem(groupName, "Scale", titleText_->worldTransform_.scale_);

	groupName = "UI_AButton";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Translation", Abutton_->GetPosition());
	globalVariables->AddItem(groupName, "EffectSpeed", AbuttonEffectParam_.speed_);
	globalVariables->AddItem(groupName, "StartScale", AbuttonEffectParam_.startScale_);
	globalVariables->AddItem(groupName, "EndScale", AbuttonEffectParam_.endScale_);

	groupName = "WarpHole";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Position", warpHolePos_);

	groupName = "WarpTransition";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Speed", warpTransitionParam_.speed_);


}

void TitleScene::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	std::string groupName = "TitleText";

	titleText_->worldTransform_.translation_ = globalVariables->GetVec3Value(groupName, "Translation");
	titleText_->worldTransform_.rotation_ = globalVariables->GetVec3Value(groupName, "Rotation");
	titleText_->worldTransform_.scale_ = globalVariables->GetVec3Value(groupName, "Scale");

	groupName = "UI_AButton";
	Abutton_->SetPosition(globalVariables->GetVec2Value(groupName, "Translation"));
	AbuttonEff_->SetPosition(Abutton_->GetPosition());
	AbuttonEffectParam_.speed_ = globalVariables->GetFloatValue(groupName, "EffectSpeed");
	AbuttonEffectParam_.startScale_ = globalVariables->GetFloatValue(groupName, "StartScale");
	AbuttonEffectParam_.endScale_ = globalVariables->GetFloatValue(groupName, "EndScale");

	groupName = "WarpHole";
	warpHolePos_ = globalVariables->GetVec3Value(groupName, "Position");
	warpTransitionParam_.endCameraPos_ = warpHolePos_;

	groupName = "WarpTransition";


}

void TitleScene::Init() {
	

	camera_.Init();
	pointLight_.Init();
	spotLight_.Init();

	Object3d::SetPointLight(&pointLight_);
	Object3d::SetSpotLight(&spotLight_);

	uint32_t AbuttonTex = TextureManager::Load("AButton.png");
	uint32_t backGroundTex = TextureManager::Load("Black.png");
	uint32_t AbuttonEffectTex = TextureManager::Load("ButtonEffect.png");
	uint32_t fadeSpriteTex = TextureManager::Load("white.png");

	std::shared_ptr<Model> titleTextModel = ModelManager::LoadOBJ("TitleText");

	titleText_.reset(Object3d::Create(titleTextModel));
	
	backGround_.reset(Sprite::Create(backGroundTex, { WinApp::kClientWidth / 2.0f, WinApp::kClientHeight / 2.0f }));
	backGround_->SetSize(Vector2(WinApp::kClientWidth, WinApp::kClientHeight));

	fadeSprite_.reset(Sprite::Create(fadeSpriteTex, { WinApp::kClientWidth / 2.0f, WinApp::kClientHeight / 2.0f }));
	fadeSprite_->SetSize(Vector2(WinApp::kClientWidth, WinApp::kClientHeight));

	Abutton_.reset(Sprite::Create(AbuttonTex, {}));
	AbuttonEff_.reset(Sprite::Create(AbuttonEffectTex, {}));

	titleEff_ = ParticleManager::Load("RunWay");
	warpHole_ = ParticleManager::Load("WarpHole");

	WarpTransitionInit();

	//調整項目の追加と代入
	SetGlobalVariables();
	ApplyGlobalVariables();
	
	//行列更新
	titleText_->worldTransform_.UpdateMatrix();
}

void TitleScene::Update() {
#ifdef _DEBUG
	ApplyGlobalVariables();
#endif // _DEBUG
	DebugGUI();

#ifdef _DEBUG

	if (Input::GetInstance()->PushKey(DIK_LCONTROL) && Input::GetInstance()->TriggerKey(DIK_2)) {
		SceneManager::GetInstance()->ChangeScene("Game");
	}
	if (Input::GetInstance()->PushKey(DIK_LCONTROL) && Input::GetInstance()->TriggerKey(DIK_3)) {
		SceneManager::GetInstance()->ChangeScene("Debug");
	}
	if (Input::GetInstance()->TriggerKey(DIK_H)) {
		warpTransitionParam_.isTransition_ = true;
	}

#endif // _DEBUG

	if (Input::GetInstance()->TriggerButton(Input::Button::A)) {
		warpTransitionParam_.isTransition_ = true;
	}

	if (warpTransitionParam_.isTransition_) {
		WarpTransitionUpdate();
	}

	UIEffectUpdate();

	for (auto& [group, particle] : titleEff_) {
		particle->particleData_.emitter_.translate.y = -1.0f;
		particle->Update();
	}
	for (auto& [group, particle] : warpHole_) {
		particle->particleData_.emitter_.translate = warpHolePos_;
		particle->Update();
	}

	titleText_->worldTransform_.UpdateMatrix();
	pointLight_.Update();
	spotLight_.Update();
	camera_.UpdateViewMatrix();
}

void TitleScene::DrawBackGround(){

	backGround_->Draw();

}

void TitleScene::DrawModel(){

	titleText_->Draw(camera_);

}

void TitleScene::DrawParticleModel(){



}

void TitleScene::DrawParticle(){

	for (auto& [group, particle] : titleEff_) {
		particle->Draw(camera_);
	}
	for (auto& [group, particle] : warpHole_) {
		particle->Draw(camera_);
	}

}

void TitleScene::DrawUI(){

	if (!warpTransitionParam_.isTransition_) {
		AbuttonEff_->Draw();
		Abutton_->Draw();
	}else {
		fadeSprite_->Draw();
	}
	
}

void TitleScene::DrawPostEffect() {



}

void TitleScene::DrawRenderTexture() {



}

void TitleScene::DebugGUI(){
#ifdef _DEBUG




#endif // _DEBUG
}

void TitleScene::UIEffectInit() {

	AbuttonEffectParam_.cycle_ = 0.0f;
	AbuttonEffectParam_.alpha_ = 1.0f;

}

void TitleScene::UIEffectUpdate() {

	AbuttonEffectParam_.cycle_ += AbuttonEffectParam_.speed_;
	AbuttonEffectParam_.cycle_ = std::fmod(AbuttonEffectParam_.cycle_, 1.0f);

	AbuttonEffectParam_.alpha_ = Lerp(AbuttonEffectParam_.cycle_, 1.0f, 0.0f);
	float scale{};
	scale = Lerp(AbuttonEffectParam_.cycle_, AbuttonEffectParam_.startScale_, AbuttonEffectParam_.endScale_);

	AbuttonEff_->SetColor(Vector4(1.0f, 1.0f, 1.0f, AbuttonEffectParam_.alpha_));
	AbuttonEff_->SetScale(scale);

}

void TitleScene::WarpTransitionInit() {

	warpTransitionParam_.param_ = 0.0f;
	warpTransitionParam_.alpha_ = 0.0f;
	warpTransitionParam_.startCameraPos_ = camera_.translation_;

}

void TitleScene::WarpTransitionUpdate() {

	warpTransitionParam_.param_ += warpTransitionParam_.speed_;
	warpTransitionParam_.param_ = std::clamp(warpTransitionParam_.param_, 0.0f, 1.0f);

	float T = Easing::easeInCirc(warpTransitionParam_.param_);
	//カメラの移動
	camera_.translation_ = Lerp(T, warpTransitionParam_.startCameraPos_, warpTransitionParam_.endCameraPos_);

	T = Easing::easeInExpo(warpTransitionParam_.param_);
	//α値の変化
	warpTransitionParam_.alpha_ = Lerp(T, 0.0f, 1.0f);

	fadeSprite_->SetColor(Vector4(1.0f, 1.0f, 1.0f, warpTransitionParam_.alpha_));

	//演出が終わったらシーンチェンジ
	if (warpTransitionParam_.param_ >= 1.0f) {
		SceneManager::GetInstance()->ChangeScene("Game");
	}

}

