#include "DebugTestScene.h"

#include "DirectXCommon.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include "Audio.h"
#include "Input.h"
#include "SceneManager.h"

void DebugTestScene::Init() {

	camera_.Init();
	pointLight_.Init();
	pointLight_.intensity_ = 0.0f;
	spotLight_.Init();
	spotLight_.intensity_ = 0.0f;

	Object3d::SetPointLight(&pointLight_);
	Object3d::SetSpotLight(&spotLight_);

	humanModel_ = ModelManager::LoadGLTF("walk");

	human_.reset(Object3d::Create(humanModel_));
	animation_.LoadAnimationFile(humanModel_->name_);

}

void DebugTestScene::Update() {

#ifdef _DEBUG

	if (Input::GetInstance()->PushKey(DIK_LCONTROL) && Input::GetInstance()->TriggerKey(DIK_1)) {
		SceneManager::GetInstance()->ChangeScene("Title");
	}
	if (Input::GetInstance()->PushKey(DIK_LCONTROL) && Input::GetInstance()->TriggerKey(DIK_2)) {
		SceneManager::GetInstance()->ChangeScene("Game");
	}
	
#endif // _DEBUG

	human_->worldTransform_.UpdateMatrix();

	
	

	camera_.UpdateViewMatrix();
	pointLight_.Update();
	spotLight_.Update();
}

void DebugTestScene::DrawBackGround() {



}

void DebugTestScene::DrawModel() {

	human_->Draw(camera_);

}

void DebugTestScene::DrawParticleModel() {



}

void DebugTestScene::DrawParticle() {



}

void DebugTestScene::DrawUI() {



}

void DebugTestScene::DrawPostEffect() {



}

void DebugTestScene::DebugGUI() {



}
