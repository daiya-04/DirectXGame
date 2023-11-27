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
#include <random>
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

Enemy* EnemyPop(std::vector<uint32_t> modelHandles,Vector3 pos);

int WINAPI WinMain(_In_ HINSTANCE,_In_opt_ HINSTANCE,_In_ LPSTR,_In_ int) {
	//D3DResourceLeakChecker leakCheck;

	WinApp* win = nullptr;
	DirectXCommon* dxCommon = nullptr;

	
	

	win = WinApp::GetInstance();
	win->CreateGameWindow(L"Ultimate3");

	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(win);

	ImGuiManager* imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(win, dxCommon);

	
	Input::GetInstance()->Initialize(win);

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
	grounds_[2]->Initialize(groundModel_, Type::Static, { 0.0f,0.0f,120.0f},{20.0f,1.0f,12.0f});

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
	uint32_t enemyNum_ = 5;
	std::list<std::unique_ptr<Enemy>> enemies_;
	uint32_t enemyBodyModel_ = ModelManager::Load("EnemyBody");
	uint32_t enemyHeadModel_ = ModelManager::Load("EnemyHead");

	std::vector<Vector3> enemyPos(enemyNum_);

	enemyPos = {
		{-70.0f,0.0f,80.0f},{-50.0f,0.0f,80.0f},
		{0.0f,0.0f,80.0f},
		{70.0f,0.0f,80.0f},{50.0f,0.0f,80.0f},
	};
	
	std::vector<uint32_t> enemyModels = {
		enemyBodyModel_,enemyHeadModel_
	};
	for (size_t index = 0; index < enemyNum_; index++) {
		enemies_.push_back(std::unique_ptr<Enemy>(EnemyPop(enemyModels, enemyPos[index])));
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

	//ロックオン
	std::unique_ptr<LockOn> lockOn_;
	lockOn_ = std::make_unique<LockOn>();
	lockOn_->Initialize();
	followCemra_->SetLockOn(lockOn_.get());
	player_->SetLockOn(lockOn_.get());

	//パーティクル
	uint32_t circle = TextureManager::Load("circle.png");
	std::unique_ptr<Particle> particle_ = std::make_unique<Particle>();
	particle_.reset(Particle::Create(circle, 50));

	std::random_device seedGenerator;
	std::mt19937 randomEngine(seedGenerator());
	const float kDeltaTime = 1.0f / 60.0f;

	std::list<Particle::ParticleData> particles_;
	Particle::Emitter emitter_{};
	emitter_.count_ = particle_->particleMaxNum_;

	/////


	
	//ウィンドウの✕ボタンが押されるまでループ
	while (true) {
		
		if (win->ProcessMessage()) { break; }
		

		imguiManager->Begin();


#ifdef _DEBUG
		

		

		
#endif // _DEBUG

		

		//更新

		Input::GetInstance()->Update();
		

		GlobalVariables::GetInstance()->Update();

		///オブジェクトの更新

		enemies_.remove_if([](const std::unique_ptr<Enemy>& enemy) {
			if (enemy->IsDead()) {
				return true;
			}
			return false;
		});

		skydome_->Update();
		for (const auto& ground : grounds_) {
			ground->Update();
		}
		player_->Update();
		goal_->Update();
		for (const auto& enemy : enemies_) {
			enemy->Update();
		}
		
		followCemra_->Update();
		lockOn_->Update(enemies_, viewProjection_);

		if (player_->IsParticle()) {
			emitter_.translate_ = player_->GetWeaponWorldPos();
			particles_.splice(particles_.end(), Particle::Emit(emitter_, randomEngine));
		}

		for (std::list<Particle::ParticleData>::iterator itParticle = particles_.begin(); itParticle != particles_.end(); itParticle++) {
			(*itParticle).worldTransform_.translation_ += (*itParticle).velocity_;
			(*itParticle).currentTime_ += kDeltaTime;
		}


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
			for (const auto& enemy : enemies_) {
				enemy->SetIsDead(true);
			}
			for (size_t index = 0; index < enemyNum_; index++) {
				enemies_.push_back(std::unique_ptr<Enemy>(EnemyPop(enemyModels, enemyPos[index])));
			}
		}

		if (player_->GetWorldPos().y <= -20.0f) {
			player_->ReStart();
			for (const auto& enemy : enemies_) {
				enemy->SetIsDead(true);
			}
			for (size_t index = 0; index < enemyNum_; index++) {
				enemies_.push_back(std::unique_ptr<Enemy>(EnemyPop(enemyModels, enemyPos[index])));
			}
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
				if (!player_->IsHit()) {
					enemy->OnCollision();
					player_->AttackHit();
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

		particle_->Draw(particles_, viewProjection_);

		Particle::postDraw();
		
		imguiManager->Draw();

		dxCommon->postDraw();

	}

	imguiManager->Finalize();

	//解放処理

	win->TerminateGameWindow();

	return 0;
}

Enemy* EnemyPop(std::vector<uint32_t> modelHandles, Vector3 pos) {

	Enemy* enemy = new Enemy();
	enemy->Initialize(modelHandles);
	enemy->SetPos(pos);

	return enemy;
}
