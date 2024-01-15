#include "SelectScene.h"

#include"Ease/Ease.h"
#include"SceneManager.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include <random>

SelectScene::~SelectScene() {}

void SelectScene::Init() {

	camera_.Init();
	camera_.translation_ = { 0.0f,2.1f,-7.0f };
	camera_.rotation_ = { 0.33f,0.0f,0.0f };

	Model_ = ModelManager::Load("box");
	seaHorseModel_ = ModelManager::Load("SeaHorse");
	

	obj_.reset(Object3d::Create(Model_));
	objWT_.Init();
	objWT_.translation_ = { 0.0f,0.0f,0.0f };

	obj2_.reset(Object3d::Create(Model_));
	objWT2_.Init();
	objWT2_.translation_ = {7.0f,0.0f,0.0f };

	obj3_.reset(Object3d::Create(Model_));
	objWT3_.Init();
	objWT3_.translation_ = { 14.0f,0.0f,0.0f };

	playerObj_.reset(Object3d::Create(seaHorseModel_));
	playerWT_.Init();
	seaHorseWT_.Init();
	playerWT_.translation_ = { 0.0f,0.0f,-3.0f };
	playerWT_.rotation_ = { 0.0f,3.14f,0.0f };
	playerWT_.scale_ = { 0.2f,0.2f,0.2f };

	seaHorseWT_.parent_ = &playerWT_;

	input_ = Input::GetInstance();
}

void SelectScene::Update() {
	DebugGUI();
	objWT_.rotation_.y += 0.02f;
	objWT2_.rotation_.y += 0.02f;
	objWT3_.rotation_.y += 0.02f;

	SelectStage();
	
	camera_.UpdateViewMatrix();

	
	playerWT_.UpdateMatrix();
	objWT_.UpdateMatrix();
	objWT2_.UpdateMatrix();
	objWT3_.UpdateMatrix();

	if (input_->TriggerKey(DIK_RETURN) || input_->TriggerButton(XINPUT_GAMEPAD_A)) {
		SceneManager::GetInstance()->ChangeScene(AbstractSceneFactory::SceneName::Game);
	}

}

void SelectScene::SelectStage(){
	
	if (input_->TriggerKey(DIK_RIGHT) || input_->TriggerButton(XINPUT_GAMEPAD_DPAD_RIGHT)) {
		rotate_ = { 1.0f,0.0f,0.0f };
		if (selectNum_ < maxStage_ - 1) {
			startPos_ = camera_.translation_.x;
			startRotate_ = 1.57f;
			
			selectNum_++;
			easeT_ = 0;
			easeRotateT_ = 0;
		}
	}
	else if (input_->TriggerKey(DIK_LEFT) || input_->TriggerButton(XINPUT_GAMEPAD_DPAD_LEFT)) {
		rotate_ = { -1.0f,0.0f,0.0f };
		if (selectNum_ > 0) {
			startPos_ = camera_.translation_.x;
			startRotate_ = 1.57f * 3.0f;
			
			selectNum_--;
			easeT_ = 0;
			easeRotateT_ = 0;
		}
	}
	

	endPos_ = selectNum_ * movePos_;
	easeT_ += addEase_;
	
	if (easeT_ > 1.0f) {
		easeRotateT_ += addRotateEase_;
		easeT_ = 1.0f;
	}
	if (easeRotateT_>1.0f){
		easeRotateT_ = 1.0f;
		endRotate_ = 3.14f;
	}

	camera_.translation_.x = Ease::Easing(Ease::EaseName::EaseOutSine, startPos_, endPos_, easeT_);
	playerWT_.translation_.x = Ease::Easing(Ease::EaseName::EaseNone, startPos_, endPos_, easeT_);
	playerWT_.rotation_.y = Ease::Easing(Ease::EaseName::EaseNone, startRotate_, endRotate_, easeRotateT_);
}

void SelectScene::DrawBackGround() {



}

void SelectScene::DrawModel() {
	playerObj_->Draw(playerWT_, camera_);
	obj_->Draw(objWT_, camera_);
	obj2_->Draw(objWT2_, camera_);
	obj3_->Draw(objWT3_, camera_);
}

void SelectScene::DrawParticleModel() {



}

void SelectScene::DrawParticle() {


}

void SelectScene::DrawUI() {



}

void SelectScene::DebugGUI() {
#ifdef _DEBUG

	ImGui::Begin("camera");

	ImGui::DragFloat3("pos", &camera_.translation_.x, 0.01f);
	ImGui::DragFloat3("rotate", &camera_.rotation_.x, 0.01f);

	ImGui::End();

	ImGui::Begin("obj");

	ImGui::DragFloat3("1", &objWT_.translation_.x, 0.01f);
	ImGui::DragFloat3("2", &objWT2_.translation_.x, 0.01f);
	ImGui::DragFloat3("3", &objWT3_.translation_.x, 0.01f);

	ImGui::End();

	ImGui::Begin("SelectNum");

	ImGui::Text("now = %d", selectNum_);
	ImGui::Text("old = %d", oldSelectNum_);
	ImGui::End();


#endif // _DEBUG
}
