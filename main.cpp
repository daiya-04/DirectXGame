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
#include "Model.h"
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

	uint32_t uv = TextureManager::Load("uvChecker.png");
	uint32_t circle = TextureManager::Load("circle.png");

	Sprite* sprite = new Sprite(uv,{50.0f,50.0f}, { 100.0f,100.0f });
	sprite->Initialize();
	sprite->SetAnchorpoint({ 0.5f,0.5f });

	float rotate = sprite->GetRotate();
	Vector2 pos = sprite->GetPosition();

	std::unique_ptr<Model> teapot = std::make_unique<Model>();
	teapot.reset(Model::LoadOBJ("teapot"));
	
	std::unique_ptr<Model> plane = std::make_unique<Model>();
	plane.reset(Model::LoadOBJ("Plane"));

	std::unique_ptr<Object3d> obj;
	obj = std::make_unique<Object3d>();
	obj.reset(Object3d::Create(teapot.get()));

	std::unique_ptr<Object3d> obj2;
	obj2 = std::make_unique<Object3d>();
	obj2.reset(Object3d::Create(plane.get()));

	std::unique_ptr<Object3d> obj3;
	obj3 = std::make_unique<Object3d>();
	obj3.reset(Object3d::Create(plane.get()));

	
	

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

	ViewProjection viewProjection;
	viewProjection.Initialize();

	

	

	
	
	WorldTransform worldTransform;
	WorldTransform worldTransform2;
	WorldTransform worldTransform3;

	
	//ウィンドウの✕ボタンが押されるまでループ
	while (true) {
		
		if (win->ProcessMessage()) { break; }
		

		imguiManager->Begin();

		input->Update();

		//更新
#ifdef _DEBUG
		ImGui::Begin("window");
		ImGui::Text("Frame rate: %6.2f fps", ImGui::GetIO().Framerate);

		ImGui::End();

		ImGui::Begin("OBJ");

		ImGui::DragFloat3("obj1", &worldTransform.translation_.x, 0.01f);
		ImGui::DragFloat3("obj2", &worldTransform2.translation_.x, 0.01f);
		ImGui::DragFloat3("obj3", &worldTransform3.translation_.x, 0.01f);

		ImGui::End();

		ImGui::Begin("Camera");

		ImGui::DragFloat3("Postion", &viewProjection.translation_.x, 0.01f);
		ImGui::DragFloat3("Rotation", &viewProjection.rotation_.x, 0.01f);

		ImGui::End();
#endif // _DEBUG

		
		if (input->PushKey(DIK_SPACE)) {
			obj2->SetModel(teapot.get());
			obj3->SetModel(teapot.get());
		}
		else {
			obj2->SetModel(plane.get());
			obj3->SetModel(plane.get());
		}
        
		
		rotate += 0.02f;
		pos.x += 1.0f;
		pos.y += 1.0f;
		//worldTransform.translation_.x += 0.01f;
		

		sprite->SetRotate(rotate);
		sprite->SetPosition(pos);
		
		
		

		worldTransform.UpdateMatrix();
		worldTransform2.UpdateMatrix();
		worldTransform3.UpdateMatrix();
		viewProjection.UpdateMatrix();


		imguiManager->End();

		//描画

		dxCommon->preDraw();

		
		
		Sprite::preDraw(dxCommon->GetCommandList());


		//sprite->Draw();

		Sprite::postDraw();

		Object3d::preDraw();


		obj->Draw(worldTransform,viewProjection);
		obj2->Draw(worldTransform2,viewProjection);
		obj3->Draw(worldTransform3, viewProjection);

		Object3d::postDraw();

		Particle::preDraw();

		

		Particle::postDraw();
		
		imguiManager->Draw();

		dxCommon->postDraw();

	}

	imguiManager->Finalize();

	//解放処理

	delete sprite;
	win->TerminateGameWindow();

	return 0;
}
