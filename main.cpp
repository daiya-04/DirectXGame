#include "WinApp.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include <dxgidebug.h>
#include "Matrix44.h"
#include "Vec3.h"
#include "Vec2.h"
#include "Vec4.h"
#include "Quaternion.h"
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

		
		Quaternion q1 = { 2.0f,3.0f,4.0f,1.0f };
		Quaternion q2 = { 1.0f,3.0f,5.0f,2.0f };

		Quaternion identity = IdentityQuaternion();

		Quaternion mul1 = q1 * q2;
		Quaternion mul2 = q2 * q1;

		
#ifdef _DEBUG

		ImGui::Begin("window");

		ImGui::Text("%.02f %.02f %.02f %.02f : Identity", identity.x, identity.y, identity.z, identity.w);

		ImGui::Text("%.02f %.02f %.02f %.02f : Conjugation", q1.Conjugation().x, q1.Conjugation().y, q1.Conjugation().z, q1.Conjugation().w);

		ImGui::Text("%.02f %.02f %.02f %.02f : Inverse", q1.Inverse().x, q1.Inverse().y, q1.Inverse().z, q1.Inverse().w);

		ImGui::Text("%.02f %.02f %.02f %.02f : Normalize", q1.Normalize().x, q1.Normalize().y, q1.Normalize().z, q1.Normalize().w);

		ImGui::Text("%.02f %.02f %.02f %.02f : Multiply q1 * q2", mul1.x, mul1.y, mul1.z, mul1.w);

		ImGui::Text("%.02f %.02f %.02f %.02f : Multiply q2 * q1", mul2.x, mul2.y, mul2.z, mul2.w);

		ImGui::Text("%.02f : Norm", q1.Length());

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