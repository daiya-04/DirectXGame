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
	Object3d::StaticInitialize(dxCommon->GetDevice(), dxCommon->GetCommandList(),WinApp::kClientWidth, WinApp::kClientHeight);

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

	//
	//vertexDataCube[0].position = { -1.0f,1.0f,-1.0f,1.0f };    //左上前
	//vertexDataCube[0].texcoord = { 0.0f,0.0f };				   
	//vertexDataCube[1].position = { 1.0f,1.0f,-1.0f,1.0f };     //右上前
	//vertexDataCube[1].texcoord = { 1.0f,0.0f };				   
	//vertexDataCube[2].position = { -1.0f,-1.0f,-1.0f,1.0f };   //左下前
	//vertexDataCube[2].texcoord = { 0.0f,1.0f };				   
	//vertexDataCube[3] = vertexDataCube[1];                     //右上前
	//vertexDataCube[4].position = { 1.0f,-1.0f,-1.0f,1.0f };    //右下前
	//vertexDataCube[4].texcoord = { 1.0f,1.0f };				   
	//vertexDataCube[5] = vertexDataCube[2];                     //左下前
	//vertexDataCube[6].position = { -1.0f,1.0f,1.0f,1.0f };     //左上奥
	//vertexDataCube[6].texcoord = {0.0f,0.0f};				   
	//vertexDataCube[7].position = vertexDataCube[0].position;   //左上前
	//vertexDataCube[7].texcoord = { 1.0f,0.0f };				   
	//vertexDataCube[8].position = { -1.0f,-1.0f,1.0f,1.0f };    //左下奥
	//vertexDataCube[8].texcoord = { 0.0f,1.0f };				   
	//vertexDataCube[9] = vertexDataCube[7];                     //左上前
	//vertexDataCube[10].position = vertexDataCube[2].position;  //左下前
	//vertexDataCube[10].texcoord = { 1.0f,1.0f };			   
	//vertexDataCube[11] = vertexDataCube[8];                    //左下奥
	//vertexDataCube[12].position = { 1.0f,1.0f,1.0f,1.0f };     //右上奥
	//vertexDataCube[12].texcoord = { 0.0f,0.0f };			   
	//vertexDataCube[13].position = vertexDataCube[6].position;  //左上奥
	//vertexDataCube[13].texcoord = { 1.0f,0.0f };			   
	//vertexDataCube[14].position = { 1.0f,-1.0f,1.0f,1.0f };    //右下奥
	//vertexDataCube[14].texcoord = { 0.0f,1.0f };			   
	//vertexDataCube[15] = vertexDataCube[13];                   //左上奥
	//vertexDataCube[16].position = vertexDataCube[8].position;  //左下奥
	//vertexDataCube[16].texcoord = { 1.0f,1.0f };			   
	//vertexDataCube[17] = vertexDataCube[14];                   //右下奥
	//vertexDataCube[18].position = vertexDataCube[1].position;  //右上前
	//vertexDataCube[18].texcoord = { 0.0f,0.0f };			   
	//vertexDataCube[19].position = vertexDataCube[12].position; //右上奥
	//vertexDataCube[19].texcoord = { 1.0f,0.0f };
	//vertexDataCube[20].position = vertexDataCube[4].position;  //右下前
	//vertexDataCube[20].texcoord = { 0.0f,1.0f };
	//vertexDataCube[21] = vertexDataCube[19];                   //右上奥
	//vertexDataCube[22].position = vertexDataCube[14].position; //右下奥
	//vertexDataCube[22].texcoord = { 1.0f,1.0f };
	//vertexDataCube[23] = vertexDataCube[20];                   //右下前
	//vertexDataCube[24] = vertexDataCube[0];                    //左上前
	//vertexDataCube[25] = vertexDataCube[13];                   //左上奥
	//vertexDataCube[26].position = vertexDataCube[18].position; //右上前
	//vertexDataCube[26].texcoord = { 0.0f,1.0f };
	//vertexDataCube[27] = vertexDataCube[25];                   //左上前
	//vertexDataCube[28].position = vertexDataCube[19].position; //右上奥
	//vertexDataCube[28].texcoord = { 1.0f,1.0f };
	//vertexDataCube[29] = vertexDataCube[26];                   //右上前
	//vertexDataCube[30].position = vertexDataCube[8].position;  //左下奥
	//vertexDataCube[30].texcoord = { 0.0f,0.0f };
	//vertexDataCube[31].position = vertexDataCube[2].position;  //左下前
	//vertexDataCube[31].texcoord = { 1.0f,0.0f };
	//vertexDataCube[32].position = vertexDataCube[14].position; //右下奥
	//vertexDataCube[32].texcoord = { 0.0f,1.0f };
	//vertexDataCube[33] = vertexDataCube[31];                   //左下前
	//vertexDataCube[34].position = vertexDataCube[4].position;  //右下前
	//vertexDataCube[34].texcoord = { 1.0f,1.0f };
	//vertexDataCube[35] = vertexDataCube[32];                   //右下奥

	Vector3 cameraPos = {0.0f,5.0f,0.0f};
	Vector3 cameraRotate = {};

	
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

		Object3d::UpdateViewMatrix(cameraRotate, cameraPos);

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