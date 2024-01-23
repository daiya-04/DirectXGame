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
	rockModel_ = ModelManager::Load("StageSelect");
	stageSelectModel_ = ModelManager::Load("Coral");
	floorModel_ = ModelManager::Load("Firld");
	skyModel_ = ModelManager::Load("skyDome");

	loadModel_ = ModelManager::Load("Wall");

	for (int i = 0; i < maxStage_; i++){
		obj_[i].reset(Object3d::Create(Model_));
		objWT_[i].Init();
		objWT_[i].translation_ = { i * 7.0f,0.0f,0.0f };
	}

	skyDomeObj_.reset(Object3d::Create(skyModel_));
	skyDomeWT_.Init();
	skyDomeWT_.scale_ = { 100.0f,100.0f,100.0f };

	floorObj_.reset(Object3d::Create(floorModel_));
	floorWT_.Init();
	floorWT_.translation_ = { 0.0f,-5.0f,0.0f };

	for (int i = 0; i < maxStage_; i++){
		rockObj_[i].reset(Object3d::Create(rockModel_));
		rockWT_[i].Init();
		rockWT_[i].translation_ = { i * 7.0f,-1.34f,-3.0f };
		rockWT_[i].scale_ = { 0.3f,0.2f,0.3f };

		stageSelectObj_[i].reset(Object3d::Create(stageSelectModel_));
		stageSelectWT_[i].Init();
		stageSelectWT_[i].translation_ = { i * 7.0f,-1.23f,-3.0f };
		stageSelectWT_[i].scale_ = { 0.3f,0.2f,0.3f };

		isStageClear_[i] = false;
	}
	loadObj_.reset(Object3d::Create(loadModel_));
	loadWT_.Init();
	loadWT_.translation_ = rockWT_[1].translation_;
	loadWT_.scale_ = { 2.32f,0.01f,0.01f };

	for (int i = 0; i < maxStage_ - 1; i++){
		sLoadObj_[i].reset(Object3d::Create(loadModel_));
		sloadWT_[i].Init();
		sloadWT_[i].translation_ = { 3.5f + (i * 7.0f),rockWT_[1].translation_.y,rockWT_[1].translation_.z };
		sloadWT_[i].scale_ = { 1.1f,0.01f,0.01f };
	}
	

	playerObj_.reset(Object3d::Create(seaHorseModel_));
	playerWT_.Init();
	seaHorseWT_.Init();
	playerWT_.translation_ = { 0.0f,0.0f,-3.0f };
	playerWT_.rotation_ = { 0.0f,3.14f,0.0f };
	playerWT_.scale_ = { 0.2f,0.2f,0.2f };

	seaHorseWT_.parent_ = &playerWT_;

	isSceneNext_ = false;

	easeT_ = 0.0f;
	easeRotateT_ = 0.0f;

	startPos_ = 0.0f;
	endPos_ = 3.14f;

	startPlayerPos_ = { 0.0f ,0.0f,0.0f };
	endPlayerPos_ = { 3.14f ,0.0f ,0.0f };

	startRotate_ = 3.14f;
	endRotate_ = 3.14f;

	cooltime_ = 0;



	input_ = Input::GetInstance();
}

void SelectScene::Update() {
	DebugGUI();
	for (int i = 0; i < maxStage_; i++){
		objWT_[i].rotation_.y += 0.02f;
	}

	SelectStage();
	EnterTheStage();
	
	camera_.UpdateViewMatrix();

	
	playerWT_.UpdateMatrix();
	for (int i = 0; i < maxStage_; i++) {
		objWT_[i].UpdateMatrix();
	}
	loadWT_.UpdateMatrix();
	for (int i = 0; i < maxStage_ - 1; i++) {
		sloadWT_[i].UpdateMatrix();
	}

	skyDomeWT_.UpdateMatrix();
	floorWT_.UpdateMatrix();
	for (int i = 0; i < 3; i++){
		rockWT_[i].UpdateMatrix();
		stageSelectWT_[i].UpdateMatrix();
	}

	if ((input_->TriggerKey(DIK_RETURN) || input_->TriggerButton(XINPUT_GAMEPAD_A)) && easeT_ == 1.0f && easeRotateT_ == 1.0f) {
		if (!isSceneNext_) {
			easeRotateT_ = 0.0f;
			startPlayerPos_ = playerWT_.translation_;
			endPlayerPos_ = objWT_[selectNum_].translation_;
			easeT_ = 0.0f;

		}
		isSceneNext_ = true;
		
	}
	if (input_->TriggerKey(DIK_0)){
		SceneManager::GetInstance()->ChangeScene(AbstractSceneFactory::SceneName::Title);
	}

}

void SelectScene::SelectStage(){
	if (isSceneNext_)
		return;

	
	if (input_->TriggerKey(DIK_RIGHT) || input_->TriggerButton(XINPUT_GAMEPAD_DPAD_RIGHT)||input_->GetTiltJoyStickRight()) {
		
		if (selectNum_ < maxStage_ - 1) {
			if (isStageClear_[selectNum_]) {
				startPos_ = camera_.translation_.x;
				startPlayerPos_.x = playerWT_.translation_.x;
				startRotate_ = 1.57f;

				selectNum_++;
				easeT_ = 0;
				easeRotateT_ = 0;
			}
		}
	}
	else if (input_->TriggerKey(DIK_LEFT) || input_->TriggerButton(XINPUT_GAMEPAD_DPAD_LEFT) || input_->GetTiltJoyStickLeft()) {
		
		if (selectNum_ > 0) {
			startPos_ = camera_.translation_.x;
			startPlayerPos_.x = playerWT_.translation_.x;
			startRotate_ = 1.57f * 3.0f;
			
			selectNum_--;
			easeT_ = 0;
			easeRotateT_ = 0;
		}
	}
	

	endPos_ = selectNum_ * movePos_;
	endPlayerPos_.x = selectNum_ * movePos_;
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
	playerWT_.translation_.x = Ease::Easing(Ease::EaseName::EaseNone, startPlayerPos_.x, endPlayerPos_.x, easeT_);
	playerWT_.rotation_.y = Ease::Easing(Ease::EaseName::EaseNone, startRotate_, endRotate_, easeRotateT_);
}

void SelectScene::EnterTheStage(){
	if (!isSceneNext_)
		return;
	
	startRotate_ = 3.14f;
	endRotate_ = 0.0f;

	easeRotateT_ += (addRotateEase_ - 0.1f);
	if (easeRotateT_ > 1.0f) {
		easeRotateT_ = 1.0f;
		cooltime_++;
		if (cooltime_>=30){
			easeT_ += addEase_;
		}		
	}
	if (easeT_ > 1.0f) {
		easeT_ = 1.0f;
	}
	playerWT_.translation_ = Ease::Easing(Ease::EaseName::EaseNone, startPlayerPos_, endPlayerPos_, easeT_);
	playerWT_.rotation_.y = Ease::Easing(Ease::EaseName::EaseOutSine, startRotate_, endRotate_, easeRotateT_);
}

void SelectScene::DrawBackGround() {



}

void SelectScene::DrawModel() {
	playerObj_->Draw(playerWT_, camera_);
	
	//loadObj_->Draw(loadWT_, camera_);
	for (int i = 0; i < maxStage_ - 1; i++) {
		if (isStageClear_[i]){
			sLoadObj_[i]->Draw(sloadWT_[i], camera_);
		}
	}
	floorObj_->Draw(floorWT_, camera_);
	skyDomeObj_->Draw(skyDomeWT_, camera_);
	for (int i = 0; i < maxStage_; i++){
		obj_[i]->Draw(objWT_[i], camera_);
		rockObj_[i]->Draw(rockWT_[i], camera_);
		if (isStageClear_[i]) {
			stageSelectObj_[i]->Draw(stageSelectWT_[i], camera_);
		}
	}
}

void SelectScene::DrawParticleModel() {



}

void SelectScene::DrawParticle() {


}

void SelectScene::DrawUI() {



}

void SelectScene::DebugGUI() {
#ifdef _DEBUG
	ImGui::Begin("Select_Scene", nullptr, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar()){
		if (ImGui::BeginMenu("camera")) {
			ImGui::DragFloat3("pos", &camera_.translation_.x, 0.01f);
			ImGui::DragFloat3("rotate", &camera_.rotation_.x, 0.01f);

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("obj")){
			ImGui::DragFloat3("1", &objWT_[0].translation_.x, 0.01f);
			ImGui::DragFloat3("2", &objWT_[1].translation_.x, 0.01f);
			ImGui::DragFloat3("3", &objWT_[2].translation_.x, 0.01f);

			ImGui::DragFloat3("Floor", &floorWT_.translation_.x, 0.01f);

			ImGui::DragFloat3("skyScale", &skyDomeWT_.scale_.x, 1.0f);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Stage_Select")) {
			ImGui::DragFloat3("Rock", &rockWT_[0].translation_.x, 0.01f);
			ImGui::DragFloat3("Rock_Scale", &rockWT_[0].scale_.x, 0.01f);
			ImGui::DragFloat3("StageSelect_Trans", &stageSelectWT_[0].translation_.x, 0.01f);
			ImGui::DragFloat3("StageSelect_Scale", &stageSelectWT_[0].scale_.x, 0.01f);
			/*ImGui::DragFloat3("Load_Trans", &loadWT_.translation_.x, 0.01f);
			ImGui::DragFloat3("Load_Scale", &loadWT_.scale_.x, 0.01f);*/
			ImGui::DragFloat3("sLoad_Trans", &sloadWT_[0].translation_.x, 0.01f);
			ImGui::DragFloat3("sLoad_Scale", &sloadWT_[0].scale_.x, 0.01f);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Player")) {
			ImGui::DragFloat3("trans", &playerWT_.translation_.x, 0.01f);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("ClearFlag")) {
			ImGui::Checkbox("stage1", &isStageClear_[0]);
			ImGui::Checkbox("stage2", &isStageClear_[1]);
			ImGui::Checkbox("stage3", &isStageClear_[2]);
			ImGui::EndMenu();
		}
		
		ImGui::EndMenuBar();
	}
	
	ImGui::End();

#endif // _DEBUG
}
