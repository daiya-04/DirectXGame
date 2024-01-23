#include "Framework.h"

#include "ImGuiManager.h"
#include "Input.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "DirectionalLight.h"
#include "Sprite.h"
#include "Object3d.h"
#include "Particle.h"
#include "GlobalVariables.h"
#include "Audio.h"


void DSFramework::Init(){
	
	WinApp::GetInstance()->CreateGameWindow(L"LE2A_12_セト_ダイヤ");

	
	DirectXCommon::GetInstance()->Initialize();
	ImGuiManager::GetInstance()->Initialize();
	
	Input::GetInstance()->Initialize();
	Audio::GetInstance()->Initialize();

	TextureManager::GetInstance()->Initialize();
	ModelManager::GetInstance()->Initialize();
	//TextureManager::Load("uvChecker.png");

	Sprite::StaticInitialize(DirectXCommon::GetInstance()->GetDevice(), WinApp::kClientWidth, WinApp::kClientHeight);
	Object3d::StaticInitialize(DirectXCommon::GetInstance()->GetDevice(), DirectXCommon::GetInstance()->GetCommandList());
	Particle::StaticInitialize(DirectXCommon::GetInstance()->GetDevice(), DirectXCommon::GetInstance()->GetCommandList());

	GlobalVariables::GetInstance()->LoadFiles();

	DirectionalLight::GetInstance()->Init();

	SceneManager::GetInstance()->Init();

}

void DSFramework::Update(){

	if (WinApp::GetInstance()->ProcessMessage()) { endRequest_ = true; }


	ImGuiManager::GetInstance()->Begin();

	Input::GetInstance()->Update();
	Audio::GetInstance()->Update();

	GlobalVariables::GetInstance()->Update();

	DirectionalLight::GetInstance()->Update();

	SceneManager::GetInstance()->Update();

}

void DSFramework::Finalize(){

	ImGuiManager::GetInstance()->Finalize();
	WinApp::GetInstance()->TerminateGameWindow();

}

void DSFramework::Run(){

	//初期化
	Init();

	//ゲームループ
	while (true) {

		//更新
		Update();

		//終了リクエストが来たら抜ける
		if (IsEndRequest()||Input::GetInstance()->TriggerKey(DIK_ESCAPE)||Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_BACK)) { break; }

		//描画
		Draw();

	}

	//ゲームの終了
	Finalize();
}
