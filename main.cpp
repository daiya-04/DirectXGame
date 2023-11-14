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

		Vector3 from0 = Vector3(1.0f, 0.7f, 0.5f).Normalize();
		Vector3 to0 = -from0;
		Vector3 from1 = Vector3(-0.6f, 0.9f, 0.2f).Normalize();
		Vector3 to1 = Vector3(0.4f, 0.7f, -0.5f).Normalize();

		Matrix4x4 rotateMatrix0 = DirectionToDirection(Vector3(1.0f, 0.0f, 0.0f).Normalize(), Vector3(1.0f, 0.0f, 0.0f).Normalize());
		Matrix4x4 rotateMatrix1 = DirectionToDirection(from0, to0);
		Matrix4x4 rotateMatrix2 = DirectionToDirection(from1, to1);
		
#ifdef _DEBUG

		ImGui::Begin("window");

		Matrix4x4 textMatrix = rotateMatrix0;

		ImGui::Text("%.03f %.03f %.03f %.03f", textMatrix.m[0][0], textMatrix.m[0][1], textMatrix.m[0][2], textMatrix.m[0][3]);
		ImGui::Text("%.03f %.03f %.03f %.03f", textMatrix.m[1][0], textMatrix.m[1][1], textMatrix.m[1][2], textMatrix.m[1][3]);
		ImGui::Text("%.03f %.03f %.03f %.03f", textMatrix.m[2][0], textMatrix.m[2][1], textMatrix.m[2][2], textMatrix.m[2][3]);
		ImGui::Text("%.03f %.03f %.03f %.03f", textMatrix.m[3][0], textMatrix.m[3][1], textMatrix.m[3][2], textMatrix.m[3][3]);

		textMatrix = rotateMatrix1;

		ImGui::Text("%.03f %.03f %.03f %.03f", textMatrix.m[0][0], textMatrix.m[0][1], textMatrix.m[0][2], textMatrix.m[0][3]);
		ImGui::Text("%.03f %.03f %.03f %.03f", textMatrix.m[1][0], textMatrix.m[1][1], textMatrix.m[1][2], textMatrix.m[1][3]);
		ImGui::Text("%.03f %.03f %.03f %.03f", textMatrix.m[2][0], textMatrix.m[2][1], textMatrix.m[2][2], textMatrix.m[2][3]);
		ImGui::Text("%.03f %.03f %.03f %.03f", textMatrix.m[3][0], textMatrix.m[3][1], textMatrix.m[3][2], textMatrix.m[3][3]);

		textMatrix = rotateMatrix2;

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