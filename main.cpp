#include "WinApp.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "GlobalVariables.h"
#include <dxgidebug.h>
#include "Matrix44.h"
#include "Vec3.h"
#include "Vec2.h"
#include "Vec4.h"
#include <wrl.h>
#include "Input.h"
#include "Sprite.h"
#include "Object3d.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include <memory>
#include "Particle.h"
#include <random>
#include <numbers>
#include <list>
#include "Audio.h"


#pragma comment(lib,"dxguid.lib")

#include "Game/Scene/SceneManager.h"


using namespace Microsoft::WRL;



struct D3DResourceLeakChecker {
	~D3DResourceLeakChecker() {
		//リリースリークチェック
		ComPtr<IDXGIDebug> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		}
	}
};

int WINAPI WinMain(_In_ HINSTANCE,_In_opt_ HINSTANCE,_In_ LPSTR,_In_ int) {
	//D3DResourceLeakChecker leakCheck;

	WinApp* win = nullptr;
	DirectXCommon* dxCommon = nullptr;

	Input* input = nullptr;
	

	win = WinApp::GetInstance();
	win->CreateGameWindow(L"2207_ダルマモドシ");

	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(win);

	ImGuiManager* imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(win, dxCommon);

	input = Input::GetInstance();
	input->Initialize(win);

	Audio::GetInstance()->Initialize();

	TextureManager::GetInstance()->Initialize();
	ModelManager::GetInstance()->Initialize();
	//TextureManager::Load("uvChecker.png");

	Sprite::StaticInitialize(dxCommon->GetDevice(),WinApp::kClientWidth,WinApp::kClientHeight);
	Object3d::StaticInitialize(dxCommon->GetDevice(), dxCommon->GetCommandList());

	Particle::StaticInitialize(dxCommon->GetDevice(), dxCommon->GetCommandList());

	GlobalVariables::GetInstance()->LoadFiles();
	////////////////////////////////
	////	ゲームで使う変数宣言	////
	////////////////////////////////
	IScene::StaticInitialize(input);

	SceneManager* sceneManager = SceneManager::GetInstace();
	sceneManager->Initialize();

	
	//ウィンドウの✕ボタンが押されるまでループ
	while (true) {
		
		if (win->ProcessMessage()) { break; }
		

		imguiManager->Begin();

        input->Update();

#ifdef _DEBUG

		GlobalVariables::GetInstance()->Update();

#endif // _DEBUG

		Audio::GetInstance()->Update();

		////////////////////////
		////	ゲーム部分	////
		////////////////////////

		sceneManager->Update();

		////////////////////////////
		////	ゲーム部分終了		////
		////////////////////////////
		


		imguiManager->End();

		//描画

		dxCommon->preDraw();


		/////////////////////
		////	描画部分	/////
		/////////////////////
		
		Object3d::preDraw();

		sceneManager->DrawModel();

		Object3d::postDraw();

		Sprite::preDraw(dxCommon->GetCommandList());
		
		sceneManager->DrawUI();

		Sprite::postDraw();

		Particle::preDraw();

		sceneManager->DrawParticle();

		Particle::postDraw();
		
		imguiManager->Draw();

		/////////////////////////
		////	描画部分終了	/////
		/////////////////////////

		dxCommon->postDraw();

	}

	//解放処理
	// エンジンの解放
	imguiManager->Finalize();
	Sprite::Finalize();
	win->TerminateGameWindow();

	return 0;
}
