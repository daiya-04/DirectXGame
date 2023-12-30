#include "GameScene.h"

#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"

GameScene::~GameScene() {}

void GameScene::Init(){

	camera_.Init();

	/// モデルの読み込み

	uint32_t skydomeModel = ModelManager::Load("skydome",false);
	uint32_t groundModel = ModelManager::Load("ground");
	uint32_t playerBodyModel = ModelManager::Load("float_Body");
	uint32_t playerHeadModel = ModelManager::Load("float_Head");
	uint32_t enemyBodyModel = ModelManager::Load("EnemyBody");
	uint32_t enemyHeadModel = ModelManager::Load("EnemyHead");

	///

	///テクスチャの読み込み



	///

	///オブジェクト初期化
	
	//天球
	skydome_ = std::make_unique<Skydome>();
	skydome_->Init(skydomeModel);

	//地面
	ground_ = std::make_unique<Ground>();
	ground_->Init(groundModel);

	//プレイヤー
	player_ = std::make_unique<Player>();
	player_->Init({ playerBodyModel,playerHeadModel });

	//敵
	uint32_t enemyNum = 3;

	std::vector<Vector3> enemyPos(enemyNum);

	enemyPos = {
		{20.0f,0.0f,10.0f},
		{-15.0f,0.0f,15.0f},
		{-5.0f,0.0f,-20.0f}
	};

	for (size_t index = 0; index < enemyNum; index++) {
		enemies_.push_back(std::unique_ptr<Enemy>(EnemyPop({ enemyBodyModel ,enemyHeadModel }, enemyPos[index])));
	}

	//追従カメラ
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Init();
	followCamera_->SetTarget(&player_->GetWorldTransform());
	player_->SetFollowCamera(followCamera_.get());

	///

}

void GameScene::Update(){
	DebugGUI();

	skydome_->Update();
	ground_->Update();

	player_->Update();
	for (const auto& enemy : enemies_) {
		enemy->Update();
	}
	followCamera_->Update();

	camera_.SetMatView(followCamera_->GetCamera().GetMatView());

	//camera_.UpdateViewMatrix();
	camera_.UpdateCameraPos();
}

void GameScene::DrawBackGround(){

	

}

void GameScene::DrawModel(){

	skydome_->Draw(camera_);
	ground_->Draw(camera_);
	player_->Draw(camera_);
	for (const auto& enemy : enemies_) {
		enemy->Draw(camera_);
	}

}

void GameScene::DrawParticleModel(){



}

void GameScene::DrawParticle(){

	player_->DrawParticle(camera_);

}

void GameScene::DrawUI(){



}

void GameScene::DebugGUI(){
#ifdef _DEBUG

	
#endif // _DEBUG
}

Enemy* GameScene::EnemyPop(std::vector<uint32_t> modelHandles, Vector3 pos) {

	Enemy* enemy = new Enemy();
	enemy->Init(modelHandles);
	enemy->SetPos(pos);

	return enemy;
}
