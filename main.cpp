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
#include <memory>

#include "Skydome.h"
#include "Ground.h"
#include "Player.h"


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

	///オブジェクトの初期化

	//天球
	std::unique_ptr<Skydome> skydome_;
	std::unique_ptr<Object3d> skydomeModel_;

	skydomeModel_.reset(Object3d::Create("skydome"));
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(skydomeModel_.get());

	//地面
	std::unique_ptr<Ground> ground_;
	std::unique_ptr<Object3d> groundModel_;

	groundModel_.reset(Object3d::Create("ground"));
	ground_ = std::make_unique<Ground>();
	ground_->Initialize(groundModel_.get());

	//プレイヤー


	///


	

	
	//ウィンドウの✕ボタンが押されるまでループ
	while (true) {
		
		if (win->ProcessMessage()) { break; }
		

		imguiManager->Begin();

		//更新

        input->Update();

		///オブジェクトの更新
		skydome_->Update();
		ground_->Update();

		///

		
		imguiManager->End();

		//描画

		dxCommon->preDraw();

		imguiManager->Draw();
		
		
		Sprite::preDraw(dxCommon->GetCommandList());
		///スプライト描画
		


		///
		Sprite::postDraw();

		
		Object3d::preDraw();
		///3dオブジェクトの描画

		skydome_->Draw();
		ground_->Draw();

		///
		Object3d::postDraw();

		
		dxCommon->postDraw();

	}

	imguiManager->Finalize();

	//解放処理
	
	
	delete dxCommon;
	Sprite::Finalize();
	Object3d::Finalize();
	win->TerminateGameWindow();

	return 0;
}