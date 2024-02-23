#include "GameScene.h"

#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include "Audio.h"
#include "Input.h"
#include "SceneManager.h"
#include "Input.h"
#include "Hit.h"
#include <random>

GameScene::~GameScene() {}

void GameScene::Init(){

	camera_.Init();

	finishCount_ = finishTime_;

	/// モデルの読み込み

	uint32_t skydomeModel = ModelManager::Load("skydome",false);
	uint32_t groundModel = ModelManager::Load("ground");
	uint32_t playerBodyModel = ModelManager::Load("float_Body");
	uint32_t playerHeadModel = ModelManager::Load("float_Head");
	enemyBodyModel_ = ModelManager::Load("EnemyBody");
	enemyHeadModel_ = ModelManager::Load("EnemyHead");
	//bulletModel_ = ModelManager::Load("EnemyBullet");

	///

	///テクスチャの読み込み

	uint32_t particleTex = TextureManager::Load("circle.png");
	uint32_t XButtonTex = TextureManager::Load("XButton.png");
	uint32_t char_AttackTex = TextureManager::Load("char_Attack.png");
	uint32_t finishTex = TextureManager::Load("finish.png");


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
	/*uint32_t enemyNum = 3;

	std::vector<Vector3> enemyPos(enemyNum);*/

	/*enemyPos = {
		{20.0f,0.0f,10.0f},
		{-15.0f,0.0f,15.0f},
		{-5.0f,0.0f,-20.0f}
	};

	for (size_t index = 0; index < enemyNum; index++) {
		enemies_.push_back(std::unique_ptr<Enemy>(EnemyPop({ enemyBodyModel_ ,enemyHeadModel_ }, enemyPos[index])));
	}*/

	for (size_t index = 0; index < spawnNum_; index++) {
		std::uniform_int_distribution<int> distPosX(-60, 60);
		std::uniform_int_distribution<int> distPosZ(50, 100);
		Vector3 spawnPos = { (float)distPosX(randomEngine),0.0f,(float)distPosZ(randomEngine) };
		enemies_.push_back(std::unique_ptr<Enemy>(EnemyPop({ enemyBodyModel_ ,enemyHeadModel_ }, spawnPos)));
	}

	Enemy::SetTarget(&player_->GetWorldTransform());

	//追従カメラ
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Init();
	followCamera_->SetTarget(&player_->GetWorldTransform());
	player_->SetFollowCamera(followCamera_.get());

	//パーティクル
	particle_ = std::make_unique<Particle>();
	particle_.reset(Particle::Create(particleTex, 100));

	//UI
	
	XButton_.reset(new Sprite(XButtonTex, {1200.0f,70.0f}, { 100.0f,100.0f }));
	XButton_->Initialize();

	char_Attack_.reset(new Sprite(char_AttackTex, {1100.0f,70.0f}, { 100.0f,50.0f }));
	char_Attack_->Initialize();

	finish_.reset(new Sprite(finishTex, { 670.0f,200.0f }, { 1280.0f,400.0f }));
	finish_->Initialize();

	///


	Update();
}

void GameScene::Update(){

	DebugGUI();

	if (finishCount_ <= 0) {
		SceneManager::GetInstance()->ChangeScene("Title");
	}
	
	if (gameCount_ > 0) {
		gameCount_--;
	}

	if (gameCount_ == 0) {
		if (!enemies_.empty()) {
			enemies_.clear();
		}
		if (!enemyBullets_.empty()) {
			enemyBullets_.clear();
		}

		finishCount_--;
		
	}

#ifdef _DEBUG

	if (Input::GetInstance()->PushKey(DIK_1)) {
		SceneManager::GetInstance()->ChangeScene("Title");
	}

#endif // _DEBUG

	


	if (gameCount_ >= 20) {
		if (--spawnCount_ <= 0) {
			spawnCount_ = spawnCoolTime_;
			for (size_t index = 0; index < spawnNum_; index++) {
				std::uniform_int_distribution<int> distPos(-60, 60);
				Vector3 spawnPos = { (float)distPos(randomEngine),0.0f,(float)distPos(randomEngine) };
				enemies_.push_back(std::unique_ptr<Enemy>(EnemyPop({ enemyBodyModel_ ,enemyHeadModel_ }, spawnPos)));
			}
		}
	}
	

	skydome_->Update();
	ground_->Update();
	for (const auto& enemy : enemies_) {
		enemy->Update();
	}
	player_->Update(enemies_);
	
	for (const auto& bullet : enemyBullets_) {
		bullet->Update();
	}
	followCamera_->Update();

	

	///衝突判定

	AABB player = {
		player_->GetWorldPos() - player_->GetSize(),
		player_->GetWorldPos() + player_->GetSize()
	};

	Sphere magicAttack = { player_->GetAttackPos(),0.0f };

	switch (player_->GetConboIndex()) {
	    case 0:
			magicAttack.radius = 2.0f;
	    	break;
		case 1:
			magicAttack.radius = 5.0f;
			break;
		case 2:
			magicAttack.radius = 12.0f;
			break;
	}

	//敵弾とプレイヤー
	for (const auto& bullet : enemyBullets_) {
		Sphere bulletHitRange = { bullet->GetWorldPos(),0.3f };
		if (IsCollision(player, bulletHitRange)) {
			bullet->OnCollision();
			player_->OnCollision();
		}
	}

	//魔法攻撃と敵
	if (player_->IsAttack()) {
		for (const auto& enemy : enemies_) {
			AABB enemyBody = {
				enemy->GetWorldPos() - enemy->GetSize(),
				enemy->GetWorldPos() + enemy->GetSize()
			};
			if (IsCollision(enemyBody, magicAttack)) {
				enemy->OnCollision();
			}
		}
	}
	

	enemies_.remove_if([](const std::unique_ptr<Enemy>& enemy) {
		if (enemy->IsDead()) {
			return true;
		}
		return false;
	});

	enemyBullets_.remove_if([](const std::unique_ptr<EnemyBullet>& bullet) {
		if (bullet->IsDead()) {
			return true;
		}
		return false;
	});

	///

	for (std::list<Particle::ParticleData>::iterator itParticle = particles_.begin(); itParticle != particles_.end(); itParticle++) {
		(*itParticle).worldTransform_.translation_ += (*itParticle).velocity_;
		(*itParticle).currentTime_++;
	}

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
	for (const auto& bullet : enemyBullets_) {
		bullet->Draw(camera_);
	}

}

void GameScene::DrawParticleModel(){



}

void GameScene::DrawParticle(){

	player_->DrawParticle(camera_);
	particle_->Draw(particles_, camera_);

}

void GameScene::DrawUI(){

	if (gameCount_ <= 0) {
		finish_->Draw();
	}
	XButton_->Draw();
	char_Attack_->Draw();
	
}

void GameScene::DebugGUI(){
#ifdef _DEBUG
  
	ImGui::Begin("window");

	int count = gameCount_ / 60;
	ImGui::InputInt("gameTime", &count);

	count = finishCount_ / 60;
	ImGui::InputInt("ScenChangeCount", &count);

	
	ImGui::DragFloat2("XButton", &pos1.x, 1.0f);
	
	ImGui::DragFloat2("char_Attack", &pos2.x, 1.0f);
	

	ImGui::End();
	
#endif // _DEBUG
}

Enemy* GameScene::EnemyPop(std::vector<uint32_t> modelHandles, Vector3 pos) {

	Enemy* enemy = new Enemy();
	enemy->Init(modelHandles);
	enemy->SetPos(pos);
	std::uniform_int_distribution<int> distTime(5, 9);
	enemy->SetCoolTime(distTime(randomEngine));
	enemy->SetGameScene(this);

	return enemy;
}

void GameScene::AddEnemyBullet(EnemyBullet* enemyBullet) {
	enemyBullets_.push_back(std::unique_ptr<EnemyBullet>(enemyBullet));
}

void GameScene::AddParticle(Particle::ParticleData particle) {
	particles_.push_back(particle);
}
