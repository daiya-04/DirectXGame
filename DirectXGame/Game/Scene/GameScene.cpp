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

	///

	///テクスチャの読み込み



	///

	///オブジェクト初期化
	
	skydome_ = std::make_unique<Skydome>();
	skydome_->Init(skydomeModel);

	ground_ = std::make_unique<Ground>();
	ground_->Init(groundModel);

	player_ = std::make_unique<Player>();
	player_->Init({ playerBodyModel,playerHeadModel });
	

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


