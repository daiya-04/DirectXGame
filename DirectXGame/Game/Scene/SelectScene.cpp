#include "SelectScene.h"

#include"Ease/Ease.h"
#include"SceneManager.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include <random>

bool SelectScene::isStageClear_[maxStage_]{};

int SelectScene::selectNum_ = 0;

SelectScene::~SelectScene() {}

void SelectScene::UIInit(){
	uint32_t pressTex = TextureManager::Load("A.png");
	uint32_t stickTex = TextureManager::Load("stick.png");
	uint32_t moveTex = TextureManager::Load("move.png");
	stageT_ = TextureManager::Load("stageT.png");
	stage1_ = TextureManager::Load("stage1.png");
	stage2_ = TextureManager::Load("stage2.png");
	stage3_ = TextureManager::Load("stage3.png");

	APress_.reset(new Sprite(pressTex, { 640.0f,250.0f }, { 64.0f,64.0f }, 0.0f, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }));
	APress_->Initialize();

	pressTrans_.Init();
	pressTrans_.translation_ = { 640.0f,315.0f,0.0f };
	pressTrans_.scale_ = { 64.0f,64.0f,0.0f };

	stick_.reset(new Sprite(stickTex, { 100.0f,600.0f }, { 64.0f,64.0f }, 0.0f, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }));
	stick_->Initialize();

	stickTrans_.Init();
	stickTrans_.translation_ = { 100.0f,620.0f,0.0f };
	stickTrans_.scale_ = { 128.0f,128.0f,0.0f };

	moveText_.reset(new Sprite(moveTex, { 100.0f,600.0f }, { 64.0f,64.0f }, 0.0f, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }));
	moveText_->Initialize();

	moveTrans_.Init();
	moveTrans_.translation_ = { 260.0f,620.0f,0.0f };
	moveTrans_.scale_ = { 192.0f,192.0f,0.0f };

	stageNameWT_.Init();
	stageNameWT_.translation_ = { 640.0f,120.0f,0.0f };
	stageNameWT_.scale_ = { 394.0f,64.0f,0.0f };

	stageName_.reset(new Sprite(stageT_, { stageNameWT_.translation_.x,stageNameWT_.translation_.y }, { stageNameWT_.scale_.x,stageNameWT_.scale_.y }
	, 0.0f, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }));
	stageName_->Initialize();


	UITimer_ = 0;
	isSelectEnd_ = false;
	isSceneNext_ = false;
}

void SelectScene::Init() {

	camera_.Init();
	camera_.translation_ = { 0.0f,2.1f,-7.0f };
	camera_.rotation_ = { 0.33f,0.0f,0.0f };

	Model_ = ModelManager::Load("selectRock");
	seaHorseModel_ = ModelManager::Load("SeaHorse");
	rockModel_ = ModelManager::Load("StageSelect");
	stageSelectModel_ = ModelManager::Load("Coral");
	floorModel_ = ModelManager::Load("Firld");
	skyModel_ = ModelManager::Load("skyDome");

	loadModel_ = ModelManager::Load("Wall");

	for (int i = 0; i < maxStage_; i++){
		obj_[i].reset(Object3d::Create(Model_));
		objWT_[i].Init();
		objWT_[i].translation_ = { i * 7.0f,-1.0f,0.0f };
		objWT_[i].scale_ = 
		{ 0.15f + i * 0.15f,
			0.2f + i * 0.2f,
			0.15f + i * 0.15f };
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

	UIInit();

	SEHandle_ = Audio::LoadWave("title.wav");

	input_ = Input::GetInstance();
}

void SelectScene::Update() {
	DebugGUI();
	for (int i = 0; i < maxStage_; i++){
		objWT_[i].rotation_.y += 0.02f;
	}

	stick_->SetPosition({ stickTrans_.translation_.x,stickTrans_.translation_.y });
	stick_->SetSize({ stickTrans_.scale_.x,stickTrans_.scale_.y });

	APress_->SetPosition({ pressTrans_.translation_.x,pressTrans_.translation_.y });
	APress_->SetSize({ pressTrans_.scale_.x,pressTrans_.scale_.y });

	moveText_->SetPosition({ moveTrans_.translation_.x,moveTrans_.translation_.y });
	moveText_->SetSize({ moveTrans_.scale_.x,moveTrans_.scale_.y });

	stageName_->SetPosition({ stageNameWT_.translation_.x,stageNameWT_.translation_.y });
	stageName_->SetSize({ stageNameWT_.scale_.x,stageNameWT_.scale_.y });

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

	stickTrans_.UpdateMatrix();
	pressTrans_.UpdateMatrix();
	moveTrans_.UpdateMatrix();
	stageNameWT_.UpdateMatrix();

	skyDomeWT_.UpdateMatrix();
	floorWT_.UpdateMatrix();
	for (int i = 0; i < maxStage_; i++){
		rockWT_[i].UpdateMatrix();
		stageSelectWT_[i].UpdateMatrix();
	}

	if ((input_->TriggerKey(DIK_RETURN) || input_->TriggerButton(XINPUT_GAMEPAD_A)) && easeT_ == 1.0f && easeRotateT_ == 1.0f) {
		if (!isSceneNext_) {
			Audio::GetInstance()->SoundPlayWave(SEHandle_, 0.5f, false);
			easeRotateT_ = 0.0f;
			startPlayerPos_ = playerWT_.translation_;
			endPlayerPos_ = objWT_[selectNum_].translation_;
			endPlayerPos_.y += 1.0f;
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

		UITimer_++;
		if (UITimer_>60){
			UITimer_ = 0;
		}

	}
	else {
		UITimer_ = 0;
	}

	camera_.translation_.x = Ease::Easing(Ease::EaseName::EaseOutSine, startPos_, endPos_, easeT_);
	playerWT_.translation_.x = Ease::Easing(Ease::EaseName::EaseNone, startPlayerPos_.x, endPlayerPos_.x, easeT_);
	playerWT_.rotation_.y = Ease::Easing(Ease::EaseName::EaseNone, startRotate_, endRotate_, easeRotateT_);
}

void SelectScene::EnterTheStage(){
	if (!isSceneNext_)
		return;
	UITimer_ = 0;
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

	if (cooltime_ >= 80 && !isSelectEnd_) {
		isSelectEnd_ = true;
		SceneManager::GetInstance()->ChangeScene(AbstractSceneFactory::SceneName::Game);
	}
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
	stick_->Draw();
	if (UITimer_>30){
		APress_->Draw();
	}
	moveText_->Draw();
	if (selectNum_ == 0) {
		stageName_->SetTextureHandle(stageT_);
	}
	else if (selectNum_ == 1) {
		stageName_->SetTextureHandle(stage1_);
	}
	else if (selectNum_ == 2) {
		stageName_->SetTextureHandle(stage2_);
	}
	else if (selectNum_ == 3) {
		stageName_->SetTextureHandle(stage3_);
	}

	stageName_->Draw();
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
			ImGui::Checkbox("stage3", &isStageClear_[3]);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Sprite")){
			ImGui::DragFloat3("stickTrans", &stickTrans_.translation_.x, 0.1f);
			ImGui::DragFloat3("stickScale", &stickTrans_.scale_.x, 0.1f);
			ImGui::DragFloat3("APressTrans", &pressTrans_.translation_.x, 0.1f);
			ImGui::DragFloat3("APressScale", &pressTrans_.scale_.x, 0.1f);
			ImGui::DragFloat3("moveTexTrans", &moveTrans_.translation_.x, 0.1f);
			ImGui::DragFloat3("moveTexScale", &moveTrans_.scale_.x, 0.1f);
			ImGui::DragFloat3("stageNameTexTrans", &stageNameWT_.translation_.x, 1.0f);
			ImGui::DragFloat3("stageNameTexScale", &stageNameWT_.scale_.x, 0.1f);
			ImGui::EndMenu();
		}
		
		ImGui::EndMenuBar();
	}
	
	ImGui::End();

#endif // _DEBUG
}

