#include "Framework.h"

#include "ImGuiManager.h"
#include "Input.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "Sprite.h"
#include "Object3d.h"
#include "Particle.h"


void DSFramework::Init(){

	win = WinApp::GetInstance();
	win->CreateGameWindow(L"LE2A_12_セト_ダイヤ");

	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(win);

	ImGuiManager::GetInstance()->Initialize(win, dxCommon);

	
	Input::GetInstance()->Initialize(win);

	TextureManager::GetInstance()->Initialize();
	ModelManager::GetInstance()->Initialize();
	//TextureManager::Load("uvChecker.png");

	Sprite::StaticInitialize(dxCommon->GetDevice(), WinApp::kClientWidth, WinApp::kClientHeight);
	Object3d::StaticInitialize(dxCommon->GetDevice(), dxCommon->GetCommandList());

	Particle::StaticInitialize(dxCommon->GetDevice(), dxCommon->GetCommandList());

}

void DSFramework::Update(){

	if (win->ProcessMessage()) { endRequest_ = true; }


	ImGuiManager::GetInstance()->Begin();

	Input::GetInstance()->Update();

}

void DSFramework::Finalize(){

	win->TerminateGameWindow();
	ImGuiManager::GetInstance()->Finalize();

}

void DSFramework::Run(){

	//初期化
	Init();

	//ゲームループ
	while (true) {

		//更新
		Update();

		//終了リクエストが来たら抜ける
		if (IsEndRequest()) { break; }

		//描画
		Draw();

	}

	//ゲームの終了
	Finalize();
}