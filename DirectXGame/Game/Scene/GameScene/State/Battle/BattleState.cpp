#include "BattleState.h"
#include "GameScene.h"
#include "ColliderManager.h"

#include <cassert>

void BattleState::Init() {
	assert(gameScene_);

	gameScene_->GetLockOn()->SetTarget(gameScene_->GetBoss());

}

void BattleState::Update() {

	gameScene_->GetGameTime()->Update();

	//プレイヤーが死んだら専用シーンへ
	if (gameScene_->GetPlayer()->IsDead()) {
		//ロックオンを外す
		gameScene_->GetLockOn()->LockOff();
		gameScene_->ChangeState(GameScene::SceneEvent::PlayerDead);
		return;
	}
	//ボスが死んだら専用シーンへ
	if (gameScene_->GetBoss()->IsDead()) {
		//ロックオンを外す
		gameScene_->GetLockOn()->LockOff();
		gameScene_->ChangeState(GameScene::SceneEvent::BossDead);
		return;
	}

	for (auto& charactor : gameScene_->GetCharactors()) {
		charactor->Update();
	}

	gameScene_->GetFollowCamera()->Update();

	///攻撃の更新
	//ボス攻撃
	for (auto& bossAttack : gameScene_->GetBossAttacks()) {
		bossAttack->Update();
	}
	//プレイヤー攻撃
	for (auto& playerAttack : gameScene_->GetPlayerAttacks()) {
		playerAttack->Update();
	}

	///衝突判定

	DaiEngine::ColliderManager::GetInstance()->CheckAllCollision();

	///

	gameScene_->GetCamera().SetMatView(gameScene_->GetFollowCamera()->GetCamera().GetMatView());

}

void BattleState::DrawUI() {

	gameScene_->GetXButtonUI()->Draw();
	gameScene_->GetAButtonUI()->Draw();
	gameScene_->GetBButtonUI()->Draw();
	gameScene_->GetCharAttackUI()->Draw();
	gameScene_->GetCharDashUI()->Draw();
	gameScene_->GetCharAvoidUI()->Draw();
	gameScene_->GetPlayer()->DrawUI();
	gameScene_->GetBoss()->DrawUI();

}
