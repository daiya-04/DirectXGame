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
	DaiEngine::GlobalVariables* globalVariables = DaiEngine::GlobalVariables::GetInstance();

	//タイトル文字の調整項目追加
	std::string groupName = "TitleText";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Translation", titleText_->worldTransform_.translation_);
	globalVariables->AddItem(groupName, "Rotation", titleText_->worldTransform_.rotation_);
	globalVariables->AddItem(groupName, "Scale", titleText_->worldTransform_.scale_);
	//AボタンUIの調整項目追加
	groupName = "UI_AButton";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Translation", Abutton_->GetPosition());
	globalVariables->AddItem(groupName, "EffectSpeed", AbuttonEffectParam_.speed_);
	globalVariables->AddItem(groupName, "StartScale", AbuttonEffectParam_.startScale_);
	globalVariables->AddItem(groupName, "EndScale", AbuttonEffectParam_.endScale_);
	//ワープホールの調整項目追加
	groupName = "WarpHole";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Position", warpHolePos_);
	//ゲーム開始演出の調整項目追加
	groupName = "WarpTransition";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Speed", warpTransitionParam_.speed_);


}

void TitleScene::ApplyGlobalVariables() {
	DaiEngine::GlobalVariables* globalVariables = DaiEngine::GlobalVariables::GetInstance();
	//タイトル文字のパラメータ設定
	std::string groupName = "TitleText";
	titleText_->worldTransform_.translation_ = globalVariables->GetVec3Value(groupName, "Translation");
	titleText_->worldTransform_.rotation_ = globalVariables->GetVec3Value(groupName, "Rotation");
	titleText_->worldTransform_.scale_ = globalVariables->GetVec3Value(groupName, "Scale");
	//AボタンUIのパラメータ設定
	groupName = "UI_AButton";
	Abutton_->SetPosition(globalVariables->GetVec2Value(groupName, "Translation"));
	AbuttonEff_->SetPosition(Abutton_->GetPosition());
	AbuttonEffectParam_.speed_ = globalVariables->GetFloatValue(groupName, "EffectSpeed");
	AbuttonEffectParam_.startScale_ = globalVariables->GetFloatValue(groupName, "StartScale");
	AbuttonEffectParam_.endScale_ = globalVariables->GetFloatValue(groupName, "EndScale");
	//ワープホールのパラメータ設定
	groupName = "WarpHole";
	warpHolePos_ = globalVariables->GetVec3Value(groupName, "Position");
	//ゲーム開始演出のカメラ移動の終了地点をワープホールの位置に
	warpTransitionParam_.endCameraPos_ = warpHolePos_;

	//ゲーム開始演出のパラメータ設定
	groupName = "WarpTransition";
	warpTransitionParam_.speed_ = globalVariables->GetFloatValue(groupName, "Speed");


}

void TitleScene::Init() {
	
	//カメラ初期化
	camera_.Init();
	camera_.SetFOV(45.0f);
	//ポイントライト初期化
	pointLight_.Init();
	//スポットライト初期化
	spotLight_.Init();
	//Object3dクラスにライトのセット
	DaiEngine::Object3d::SetPointLight(&pointLight_);
	DaiEngine::Object3d::SetSpotLight(&spotLight_);

	///テクスチャ読み込み
	uint32_t AbuttonTex = DaiEngine::TextureManager::Load("AButton.png");
	uint32_t backGroundTex = DaiEngine::TextureManager::Load("Black.png");
	uint32_t AbuttonEffectTex = DaiEngine::TextureManager::Load("ButtonEffect.png");
	uint32_t fadeSpriteTex = DaiEngine::TextureManager::Load("white.png");
	///

	//モデル読み込み
	std::shared_ptr<DaiEngine::Model> titleTextModel = DaiEngine::ModelManager::LoadOBJ("TitleText");

	//タイトル文字初期化
	titleText_.reset(DaiEngine::Object3d::Create(titleTextModel));
	
	//背景初期化
	backGround_.reset(DaiEngine::Sprite::Create(backGroundTex, { DaiEngine::WinApp::kClientWidth / 2.0f,DaiEngine::WinApp::kClientHeight / 2.0f }));
	backGround_->SetSize(Vector2(DaiEngine::WinApp::kClientWidth, DaiEngine::WinApp::kClientHeight));

	//フェード用スプライト初期化
	fadeSprite_.reset(DaiEngine::Sprite::Create(fadeSpriteTex, { DaiEngine::WinApp::kClientWidth / 2.0f,DaiEngine::WinApp::kClientHeight / 2.0f }));
	fadeSprite_->SetSize(Vector2(DaiEngine::WinApp::kClientWidth, DaiEngine::WinApp::kClientHeight));

	//AボタンUI初期化
	Abutton_.reset(DaiEngine::Sprite::Create(AbuttonTex, {}));
	AbuttonEff_.reset(DaiEngine::Sprite::Create(AbuttonEffectTex, {}));

	///エフェクト読み込み
	//道
	runWay_ = ParticleManager::Load("RunWay");
	//ワープホール
	warpHole_ = ParticleManager::Load("WarpHole");

	//ゲーム開始演出初期化
	WarpTransitionInit();

	//調整項目の追加と代入
	SetGlobalVariables();
	ApplyGlobalVariables();
	
	//行列更新
	titleText_->worldTransform_.UpdateMatrix();
}

void TitleScene::Update() {
	DebugGUI();

#ifdef _DEBUG
	//調整項目代入
	ApplyGlobalVariables();
	//デバッグ用シーン切り替えコマンド
	if (DaiEngine::Input::GetInstance()->PushKey(DIK_LCONTROL) && DaiEngine::Input::GetInstance()->TriggerKey(DIK_2)) {
		DaiEngine::SceneManager::GetInstance()->ChangeScene("Game");
	}
	if (DaiEngine::Input::GetInstance()->PushKey(DIK_LCONTROL) && DaiEngine::Input::GetInstance()->TriggerKey(DIK_3)) {
		DaiEngine::SceneManager::GetInstance()->ChangeScene("Debug");
	}
	//デバッグ用ゲーム開始演出開始
	if (DaiEngine::Input::GetInstance()->TriggerKey(DIK_H)) {
		warpTransitionParam_.isTransition_ = true;
	}

#endif // _DEBUG
	//Aボタンでゲーム開始
	if (DaiEngine::Input::GetInstance()->TriggerButton(DaiEngine::Input::Button::A)) {
		warpTransitionParam_.isTransition_ = true;
	}

	if (warpTransitionParam_.isTransition_) {
		//ゲーム開始演出
		WarpTransitionUpdate();
	}

	//UIエフェクト更新
	UIEffectUpdate();

	///エフェクト更新
	//道
	for (auto& [group, particle] : runWay_) {
		particle->particleData_.emitter_.translate = runWayPos_;
		particle->Update();
	}
	//ワープホール
	for (auto& [group, particle] : warpHole_) {
		particle->particleData_.emitter_.translate = warpHolePos_;
		particle->Update();
	}

	//行列更新
	titleText_->worldTransform_.UpdateMatrix();
	
	//ライト更新
	pointLight_.Update();
	spotLight_.Update();

	//カメラ更新
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

	for (auto& [group, particle] : runWay_) {
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
	//1.0fを超えたら0.0fから
	AbuttonEffectParam_.cycle_ = std::fmod(AbuttonEffectParam_.cycle_, 1.0f);

	//徐々に透明に
	AbuttonEffectParam_.alpha_ = Lerp(AbuttonEffectParam_.cycle_, 1.0f, 0.0f);
	float scale{};
	//少しずつ大きく
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
		DaiEngine::SceneManager::GetInstance()->ChangeScene("Game");
	}

}

