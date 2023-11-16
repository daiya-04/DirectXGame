#include "WinApp.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
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
#include <memory>
#include "Particle.h"
#include <random>
#include <numbers>
#include <list>


#pragma comment(lib,"dxguid.lib")

#include "Game/Scene/SceneManager.h"
#include "Game/Object/IObject.h"


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
	win->CreateGameWindow(L"LE2A_12_セト_ダイヤ");

	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(win);

	ImGuiManager* imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(win, dxCommon);

	input = Input::GetInstance();
	input->Initialize(win);

	TextureManager::GetInstance()->Initialize();
	//TextureManager::Load("uvChecker.png");

	Sprite::StaticInitialize(dxCommon->GetDevice(),WinApp::kClientWidth,WinApp::kClientHeight);
	Object3d::StaticInitialize(dxCommon->GetDevice(), dxCommon->GetCommandList());

	Particle::StaticInitialize(dxCommon->GetDevice(), dxCommon->GetCommandList());
	////////////////////////////////
	////	ゲームで使う変数宣言	////
	////////////////////////////////
	IScene::StaticInitialize(input);
	IObject::StaticInitialize(input);


	
	//ウィンドウの✕ボタンが押されるまでループ
	while (true) {
		
		if (win->ProcessMessage()) { break; }
		

		imguiManager->Begin();

        input->Update();

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
		
		Sprite::preDraw(dxCommon->GetCommandList());


		//sprite->Draw();

		Sprite::postDraw();

		Object3d::preDraw();


		//obj->Draw(worldTransform,viewProjection);
		//plane->Draw(worldTransformPlane,viewProjection);

		Object3d::postDraw();

		Particle::preDraw();
		Sprite::preDraw(dxCommon->GetCommandList());

		sceneManager->DrawUI();
		sceneManager->DrawUI();

		Particle::postDraw();
		
		imguiManager->Draw();

		/////////////////////////
		////	描画部分終了	/////
		/////////////////////////

		dxCommon->postDraw();

	}

	imguiManager->Finalize();

	//解放処理
	// エンジンの解放
	imguiManager->Finalize();
	Sprite::Finalize();
	Object3d::Finalize();
	win->TerminateGameWindow();

	return 0;
}
