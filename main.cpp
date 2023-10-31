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


#pragma comment(lib,"dxguid.lib")



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
	D3DResourceLeakChecker leakCheck;

	WinApp* win = nullptr;
	DirectXCommon* dxCommon = nullptr;

	Input* input = nullptr;
	

	win = WinApp::GetInstance();
	win->CreateGameWindow(L"Engine");

	dxCommon = new DirectXCommon();
	dxCommon->Initialize(win);

	ImGuiManager* imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(win, dxCommon);

	input = Input::GetInstance();
	input->Initialize(win);

	Sprite::StaticInitialize(dxCommon->GetDevice(),WinApp::kClientWidth,WinApp::kClientHeight);
	Object3d::StaticInitialize(dxCommon->GetDevice(), dxCommon->GetCommandList());


	////////////////////////////////
	////	ゲームで使う変数宣言	////
	////////////////////////////////


	Sprite* sprite = new Sprite({50.0f,50.0f}, { 100.0f,100.0f });
	sprite->Initialize();
	sprite->SetAnchorpoint({ 0.5f,0.5f });

	float rotate = sprite->GetRotate();
	Vector2 pos = sprite->GetPosition();

	Object3d* obj = Object3d::Create("teapot");
	Object3d* plane = Object3d::Create("Plane");


	ViewProjection viewProjection;
	viewProjection.Initialize();
	
	WorldTransform worldTransform;
	WorldTransform worldTransformPlane;
	worldTransformPlane.parent_ = &worldTransform;
	

	////////////////////////////////////
	////	ゲームで使う変数宣言終了	////
	////////////////////////////////////



	//ウィンドウの✕ボタンが押されるまでループ
	while (true) {
		
		if (win->ProcessMessage()) { break; }
		

		imguiManager->Begin();

		//更新

        input->Update();

		////////////////////////
		////	ゲーム部分	////
		////////////////////////
		

		if (input->PushKey(DIK_D)) {
			OutputDebugStringA("Hit D\n");
		}
		if (input->TriggerKey(DIK_SPACE)) {
			OutputDebugStringA("Shot!\n");
		}

		XINPUT_STATE joyState{};

		if (Input::GetInstance()->TriggerButton(XINPUT_GAMEPAD_A)) {
			
		}
		
		rotate += 0.02f;
		pos.x += 1.0f;
		pos.y += 1.0f;
		worldTransform.translation_.x += 0.01f;
		

		sprite->SetRotate(rotate);
		sprite->SetPosition(pos);
		

		worldTransform.UpdateMatrix();
		worldTransformPlane.UpdateMatrix();


		////////////////////////////
		////	ゲーム部分終了		////
		////////////////////////////



		imguiManager->End();

		//描画

		dxCommon->preDraw();

		imguiManager->Draw();
		
		/////////////////////
		////	描画部分	/////
		/////////////////////



		Sprite::preDraw(dxCommon->GetCommandList());

		sprite->Draw();

		Sprite::postDraw();

		Object3d::preDraw();

		obj->Draw(worldTransform,viewProjection);
		plane->Draw(worldTransformPlane,viewProjection);

		Object3d::postDraw();

		/////////////////////////
		////	描画部分終了	/////
		/////////////////////////
		
		dxCommon->postDraw();

	}


	//解放処理
	delete plane;
	delete obj;
	delete sprite;


	// エンジンの解放
	imguiManager->Finalize();
	delete dxCommon;
	Sprite::Finalize();
	Object3d::Finalize();
	win->TerminateGameWindow();

	return 0;
}