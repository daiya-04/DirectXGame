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
	
	//ウィンドウの✕ボタンが押されるまでループ
	while (true) {
		
		if (win->ProcessMessage()) { break; }
		

		imguiManager->Begin();

		//更新


		

        input->Update();

		Vector3 axis = { 1.0f,1.0f,1.0f };
		axis = axis.Normalize();
		float angle = 0.44f;
		Matrix4x4 rotateMatrix = MakeRotateAxisAngle(axis, angle);
		
#ifdef _DEBUG

		ImGui::Begin("window");

		Matrix4x4 textMatrix = rotateMatrix;

		ImGui::Text("%.03f %.03f %.03f %.03f", textMatrix.m[0][0], textMatrix.m[0][1], textMatrix.m[0][2], textMatrix.m[0][3]);
		ImGui::Text("%.03f %.03f %.03f %.03f", textMatrix.m[1][0], textMatrix.m[1][1], textMatrix.m[1][2], textMatrix.m[1][3]);
		ImGui::Text("%.03f %.03f %.03f %.03f", textMatrix.m[2][0], textMatrix.m[2][1], textMatrix.m[2][2], textMatrix.m[2][3]);
		ImGui::Text("%.03f %.03f %.03f %.03f", textMatrix.m[3][0], textMatrix.m[3][1], textMatrix.m[3][2], textMatrix.m[3][3]);

		ImGui::End();

#endif // _DEBUG
		

		imguiManager->End();

		//描画

		dxCommon->preDraw();

		
		
		Sprite::preDraw(dxCommon->GetCommandList());

		

		Sprite::postDraw();

		Object3d::preDraw();

		

		Object3d::postDraw();

		imguiManager->Draw();

		dxCommon->postDraw();

	}

	imguiManager->Finalize();

	//解放処理
	
	win->TerminateGameWindow();

	return 0;
}