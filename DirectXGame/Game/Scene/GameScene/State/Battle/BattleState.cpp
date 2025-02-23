#include "BattleState.h"
#include "GameScene.h"
#include "ColliderManager.h"

#include <cassert>

void BattleState::Init() {
	assert(gameScene_);

	gameScene_->GetLockOn()->SetTarget(gameScene_->GetBoss());

}

void BattleState::Update() {

	if (gameScene_->GetPlayer()->IsDead()) {
		gameScene_->ChangeState(GameScene::SceneEvent::PlayerDead);
		gameScene_->GetLockOn()->LockOff();
		return;
	}
	if (gameScene_->GetBoss()->IsDead()) {
		gameScene_->ChangeState(GameScene::SceneEvent::BossDead);
		gameScene_->GetLockOn()->LockOff();
		return;
	}

	auto& charactors = gameScene_->GetCharactors();

	for (auto& charactor : charactors) {
		charactor->Update();
	}

	gameScene_->GetFollowCamera()->Update();

	gameScene_->GetElementBall()->Update();
	gameScene_->GetGroundFlare()->Update();
	gameScene_->GetPlasmaShot()->Update();
	gameScene_->GetIcicle()->Update();

	///衝突判定

	DaiEngine::ColliderManager::GetInstance()->CheckAllCollision();

	///

	gameScene_->GetCamera().SetMatView(gameScene_->GetFollowCamera()->GetCamera().GetMatView());

}

void BattleState::DrawUI() {

	gameScene_->GetXButtonUI()->Draw();
	gameScene_->GetAButtonUI()->Draw();
	gameScene_->GetCharAttackUI()->Draw();
	gameScene_->GetCharDashUI()->Draw();
	gameScene_->GetPlayer()->DrawUI();
	gameScene_->GetBoss()->DrawUI();

}
