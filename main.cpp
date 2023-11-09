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
#include "TextureManager.h"
#include "GlobalVariables.h"

#include "Skydome.h"
#include "Ground.h"
#include "Player.h"
#include "Hit.h"
#include "Goal.h"
#include "Enemy.h"
#include "FollowCamera.h"


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
	win->CreateGameWindow(L"Ultimate1");

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
	///オブジェクトの初期化

	ViewProjection viewProjection_;
	viewProjection_.Initialize();

	//天球
	std::unique_ptr<Skydome> skydome_;
	std::unique_ptr<Object3d> skydomeModel_;

	skydomeModel_.reset(Object3d::Create("skydome"));
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(skydomeModel_.get());

	//地面
	std::vector<std::unique_ptr<Ground>> grounds_;
	std::unique_ptr<Object3d> groundModel1_;
	std::unique_ptr<Object3d> groundModel2_;
	std::unique_ptr<Object3d> groundModel3_;

	groundModel1_.reset(Object3d::Create("ground"));
	groundModel2_.reset(Object3d::Create("ground"));
	groundModel3_.reset(Object3d::Create("ground"));
	for (size_t index = 0; index < 3; index++) {
		grounds_.push_back(std::make_unique<Ground>());
	}
	grounds_[0]->Initialize(groundModel1_.get(), Type::Static, { 0.0f,0.0f,0.0f }, { 1.5f,1.0f,1.5f });
	grounds_[1]->Initialize(groundModel2_.get(), Type::Dynamic, { 0.0f,0.0f,grounds_[0]->GetWorldPos().z + grounds_[0]->GetSize().z * 3.0f}, { 1.5f,1.0f,1.5f });
	grounds_[2]->Initialize(groundModel3_.get(), Type::Static, { 0.0f,0.0f,80.0f},{10.0f,1.0f,6.0f});

	//プレイヤー
	std::unique_ptr<Player> player_;
	std::unique_ptr<Object3d> playerHaedModel_;
	std::unique_ptr<Object3d> playerBodyModel_;

	std::unique_ptr<Object3d> weaponModel_;

	playerHaedModel_.reset(Object3d::Create("float_Head"));
	playerBodyModel_.reset(Object3d::Create("float_Body"));
	weaponModel_.reset(Object3d::Create("Stamp"));
	std::vector<Object3d*> playerModels = {
		playerBodyModel_.get(),playerHaedModel_.get(),weaponModel_.get()
	};
	player_ = std::make_unique<Player>();
	player_->Initialize(playerModels);
	

	//敵
	std::unique_ptr<Enemy> enemy_;
	std::unique_ptr<Object3d> enemyBodyModel_;
	std::unique_ptr<Object3d> enemyHeadModel_;

	enemyBodyModel_.reset(Object3d::Create("EnemyBody"));
	enemyHeadModel_.reset(Object3d::Create("EnemyHead"));
	std::vector<Object3d*> enemyModels = {
		enemyBodyModel_.get(),enemyHeadModel_.get()
	};
	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize(enemyModels);
	enemy_->SetCenter(grounds_[2]->GetWorldPos());

	//ゴール
	std::unique_ptr<Goal> goal_;
	std::unique_ptr<Object3d> goalModel_;

	goalModel_.reset(Object3d::Create("goal"));
	goal_ = std::make_unique<Goal>();
	goal_->Initialize(goalModel_.get(), grounds_[2]->GetWorldPos());

	//追従カメラ
	std::unique_ptr<FollowCamera> followCemra_;
	followCemra_ = std::make_unique<FollowCamera>();
	followCemra_->Initialize();
	followCemra_->SetTarget(&player_->GetWorldTransform());
	player_->SetViewProjection(&followCemra_->GetViewProjection());

	///


	

	
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
		enemy_->Update();
		goal_->Update();
		followCemra_->Update();



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

		AABB enemy = {
			enemy_->GetWorldPos() - enemy_->GetSize(),
			enemy_->GetWorldPos() + enemy_->GetSize()
		};

		Sphere stamp = { player_->GetWeaponWorldPos(),1.0f };

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
		
		if (IsCollision(player, enemy)) {
			player_->ReStart();
		}

		if (IsCollision(enemy, stamp)) {
			player_->ReStart();
		}

		///

		viewProjection_.matView_ = followCemra_->GetViewProjection().matView_;

		
		
		
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
		enemy_->Draw(viewProjection_);
		goal_->Draw(viewProjection_);

		///
		Object3d::postDraw();

		imguiManager->Draw();
		
		dxCommon->postDraw();

	}

	imguiManager->Finalize();

	//解放処理
	
	
	Sprite::Finalize();
	Object3d::Finalize();
	win->TerminateGameWindow();

	return 0;
}