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
#include "ModelManager.h"
#include <memory>
#include "Particle.h"
#include <random>
#include <numbers>
#include <list>


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
	

	win = WinApp::GetInstance();
	win->CreateGameWindow(L"LE2A_12_セト_ダイヤ");

	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(win);

	ImGuiManager* imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(win, dxCommon);

	
	Input::GetInstance()->Initialize(win);

	TextureManager::GetInstance()->Initialize();
	ModelManager::GetInstance()->Initialize();

	Sprite::StaticInitialize(dxCommon->GetDevice(),WinApp::kClientWidth,WinApp::kClientHeight);
	Object3d::StaticInitialize(dxCommon->GetDevice(), dxCommon->GetCommandList());

	Particle::StaticInitialize(dxCommon->GetDevice(), dxCommon->GetCommandList());


	
	//ウィンドウの✕ボタンが押されるまでループ
	while (true) {
		
		if (win->ProcessMessage()) { break; }
		

		imguiManager->Begin();

		Input::GetInstance()->Update();

		//更新
		Vector3 axis = { 1.0f,0.4f,-0.2f };
		Quaternion rotation = MakwRotateAxisAngleQuaternion(axis.Normalize(), 0.45f);
		Vector3 pointY = { 2.1f,-0.9f,1.3f };
		Matrix4x4 rotateMatrix = rotation.MakeRotateMatrix();
		Vector3 rotateByQuaternion = RotateVector(pointY, rotation);
		Vector3 rotateByMatri = Transform(pointY, rotateMatrix);


#ifdef _DEBUG
		ImGui::Begin("Result");

		ImGui::Text("%.02f %.02f %.02f %.02f  : rotation", rotation.x, rotation.y, rotation.z, rotation.w);
		ImGui::NewLine();

		Matrix4x4 textMatrix = rotateMatrix;

		ImGui::Text("rotateMatrix");
		ImGui::Text("%.03f %.03f %.03f %.03f", textMatrix.m[0][0], textMatrix.m[0][1], textMatrix.m[0][2], textMatrix.m[0][3]);
		ImGui::Text("%.03f %.03f %.03f %.03f", textMatrix.m[1][0], textMatrix.m[1][1], textMatrix.m[1][2], textMatrix.m[1][3]);
		ImGui::Text("%.03f %.03f %.03f %.03f", textMatrix.m[2][0], textMatrix.m[2][1], textMatrix.m[2][2], textMatrix.m[2][3]);
		ImGui::Text("%.03f %.03f %.03f %.03f", textMatrix.m[3][0], textMatrix.m[3][1], textMatrix.m[3][2], textMatrix.m[3][3]);
		ImGui::NewLine();

		ImGui::Text("%.02f %.02f %.02f  : rotateByQuaternion", rotateByQuaternion.x, rotateByQuaternion.y, rotateByQuaternion.z);
		ImGui::NewLine();

		ImGui::Text("%.02f %.02f %.02f  : rotateByMatri", rotateByMatri.x, rotateByMatri.y, rotateByMatri.z);
		ImGui::NewLine();
		
		ImGui::End();

		
#endif // _DEBUG

		
		


		imguiManager->End();

		//描画

		dxCommon->preDraw();

		
		
		Sprite::preDraw(dxCommon->GetCommandList());


		//sprite->Draw();

		Sprite::postDraw();

		Object3d::preDraw();



		Object3d::postDraw();

		Particle::preDraw();

		

		Particle::postDraw();
		
		imguiManager->Draw();

		dxCommon->postDraw();

	}

	imguiManager->Finalize();

	//解放処理
	win->TerminateGameWindow();

	return 0;
}
