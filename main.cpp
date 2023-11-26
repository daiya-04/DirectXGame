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
#include "ModelManager.h"
#include <memory>
#include <list>
#include "Particle.h"
#include "GlobalVariables.h"

#include "Skydome.h"
#include "Ground.h"
#include "Player.h"
#include "Hit.h"
#include "Goal.h"
#include "Enemy.h"
#include "FollowCamera.h"
#include "LockOn.h"



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
	win->CreateGameWindow(L"Ultimate3");

	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(win);

	ImGuiManager* imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(win, dxCommon);

	input = Input::GetInstance();
	input->Initialize(win);

	TextureManager::GetInstance()->Initialize();
	ModelManager::GetInstance()->Initialize();
	//TextureManager::Load("uvChecker.png");

	

	Sprite::StaticInitialize(dxCommon->GetDevice(),WinApp::kClientWidth,WinApp::kClientHeight);
	Object3d::StaticInitialize(dxCommon->GetDevice(), dxCommon->GetCommandList());
	Particle::StaticInitialize(dxCommon->GetDevice(), dxCommon->GetCommandList());

	GlobalVariables::GetInstance()->LoadFiles();

	///オブジェクトの初期化

	ViewProjection viewProjection_;
	viewProjection_.Initialize();

	//天球
	std::unique_ptr<Skydome> skydome_ = std::make_unique<Skydome>();
	uint32_t skydomeModel_ = ModelManager::Load("skydome",false);
	skydome_->Initialize(skydomeModel_);

	//地面
	std::vector<std::unique_ptr<Ground>> grounds_;
	uint32_t groundModel_ = ModelManager::Load("ground");

	
	for (size_t index = 0; index < 3; index++) {
		grounds_.push_back(std::make_unique<Ground>());
	}
	grounds_[0]->Initialize(groundModel_, Type::Static, { 0.0f,0.0f,0.0f }, { 1.5f,1.0f,1.5f });
	grounds_[1]->Initialize(groundModel_, Type::Dynamic, { 0.0f,0.0f,grounds_[0]->GetWorldPos().z + grounds_[0]->GetSize().z * 3.0f}, { 1.5f,1.0f,1.5f });
	grounds_[2]->Initialize(groundModel_, Type::Static, { 0.0f,0.0f,80.0f},{20.0f,1.0f,6.0f});

	////プレイヤー
	std::unique_ptr<Player> player_;
	uint32_t playerHaedModel_ = ModelManager::Load("float_Head");
	uint32_t playerBodyModel_ = ModelManager::Load("float_Body");

	uint32_t weaponModel_ = ModelManager::Load("Stamp");

	
	std::vector<uint32_t> playerModels = {
		playerBodyModel_,playerHaedModel_,weaponModel_
	};
	player_ = std::make_unique<Player>();
	player_->Initialize(playerModels);
	

	//敵
	uint32_t enemyNum_ = 1;
	std::list<std::unique_ptr<Enemy>> enemies_;
	uint32_t enemyBodyModel_ = ModelManager::Load("EnemyBody");
	uint32_t enemyHeadModel_ = ModelManager::Load("EnemyHead");

	
	std::vector<uint32_t> enemyModels = {
		enemyBodyModel_,enemyHeadModel_
	};
	for (size_t index = 0; index < enemyNum_; index++) {
		Enemy* enemy = new Enemy();
		enemy->Initialize(enemyModels);
		enemy->SetPos({ 0.0f,4.0f * index,0.0f });
		enemies_.push_back(std::unique_ptr<Enemy>(enemy));
	}
	
	

	//ゴール
	std::unique_ptr<Goal> goal_;
	uint32_t goalModel_ = ModelManager::Load("goal");

	goal_ = std::make_unique<Goal>();
	goal_->Initialize(goalModel_);

	

	//追従カメラ
	std::unique_ptr<FollowCamera> followCemra_;
	followCemra_ = std::make_unique<FollowCamera>();
	followCemra_->Initialize();
	followCemra_->SetTarget(&player_->GetWorldTransform());
	player_->SetFollowCamera(followCemra_.get());
	player_->SetViewProjection(&followCemra_->GetViewProjection());

	//
	std::unique_ptr<LockOn> lockOn_;
	lockOn_ = std::make_unique<LockOn>();
	lockOn_->Initialize();
	followCemra_->SetLockOn(lockOn_.get());
	player_->SetLockOn(lockOn_.get());

	/////


	
	//ウィンドウの✕ボタンが押されるまでループ
	while (true) {
		
		if (win->ProcessMessage()) { break; }
		

		imguiManager->Begin();


#ifdef _DEBUG
		

		/*ImGui::Begin("Camera");

		ImGui::DragFloat3("translation", &viewProjection_.translation_.x, 0.1f);
		ImGui::DragFloat3("rotation", &viewProjection_.rotation_.x, 0.01f);
		
		ImGui::End();*/

		
#endif // _DEBUG

		

		//更新

        input->Update();

		GlobalVariables::GetInstance()->Update();

		///オブジェクトの更新

		

		skydome_->Update();
		for (const auto& ground : grounds_) {
			ground->Update();
		}
		player_->Update();
		goal_->Update();
		for (const auto& enemy : enemies_) {
			enemy->SetCenter({ goal_->GetWorldPos().x,0.0f,goal_->GetWorldPos().z });
			enemy->Update();
		}
		
		followCemra_->Update();
		lockOn_->Update(enemies_, viewProjection_);


		///

		///衝突判定
		
		AABB player = {
			player_->GetWorldPos() - player_->GetSize(),
			player_->GetWorldPos() + player_->GetSize(),
		};

		AABB goal = {
			goal_->GetWorldPos() - goal_->GetSize(),
			goal_->GetWorldPos() + goal_->GetSize()
		};

		

		Sphere stamp = { player_->GetWeaponWorldPos(),2.0f };

		for (const auto& ground : grounds_) {
			AABB aabbGround = {
				ground->GetWorldPos() - ground->GetSize(),
				ground->GetWorldPos() + ground->GetSize(),
			};
			if (IsCollision(player, aabbGround)) {
				player_->OnGround();
				if (ground->GetType() == Type::Dynamic) {
					//player_->SetTranslationParent(&ground->GetWorldTransform());
					player_->AddTransform(ground->GetVelocity());
				}
			}
			else {
				if (ground->GetType() == Type::Dynamic) {
					player_->SetTranslationParent(nullptr);
				}
			}
		}

		if (IsCollision(player, goal)) {
			player_->ReStart();
		}
		
		for (const auto& enemy : enemies_) {
			AABB enemyAabb = {
			enemy->GetWorldPos() - enemy->GetSize(),
			enemy->GetWorldPos() + enemy->GetSize()
			};
			if (IsCollision(player, enemyAabb)) {
				player_->ReStart();
			}
		}
		
		for (const auto& enemy : enemies_) {
			AABB enemyAabb = {
			enemy->GetWorldPos() - enemy->GetSize(),
			enemy->GetWorldPos() + enemy->GetSize()
			};
			if (IsCollision(enemyAabb, stamp)) {
				if (player_->IsAttack()) {
					enemy->OnCollision();
				}
			}
		}

		

		///

		viewProjection_.matView_ = followCemra_->GetViewProjection().matView_;

		
		//viewProjection_.UpdateViewMatrix();
		
		imguiManager->End();

		//描画

		dxCommon->preDraw();

		
		Sprite::preDraw(dxCommon->GetCommandList());
		///スプライト描画
		

		///
		Sprite::postDraw();

		
		Object3d::preDraw();
		///3dオブジェクトの描画

		skydome_->Draw(viewProjection_);
		for (const auto& ground : grounds_) {
			ground->Draw(viewProjection_);
		}
		
		player_->Draw(viewProjection_);
		for (const auto& enemy : enemies_) {
			enemy->Draw(viewProjection_);
		}
		
		goal_->Draw(viewProjection_);

		///
		Object3d::postDraw();

		Sprite::preDraw(dxCommon->GetCommandList());
		///スプライト描画

		lockOn_->Draw();

		///
		Sprite::postDraw();


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
