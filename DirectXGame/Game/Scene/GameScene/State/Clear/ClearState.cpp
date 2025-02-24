#include "ClearState.h"
#include "GameScene.h"
#include "SceneManager.h"

#include <cassert>


void ClearState::Init() {

	assert(gameScene_);

}

void ClearState::Update() {

	//一定時間たったらタイトルへ
	if (--finishCount_ <= 0) {
		DaiEngine::SceneManager::GetInstance()->ChangeScene("Title");
	}

	gameScene_->GetPlayer()->Update();
	gameScene_->GetFollowCamera()->Update();

	for (auto& playerAttack : gameScene_->GetPlayerAttacks()) {
		playerAttack->Update();
	}

	gameScene_->GetCamera().SetMatView(gameScene_->GetFollowCamera()->GetCamera().GetMatView());
}

void ClearState::DrawUI() {
	gameScene_->GetFinishUI()->Draw();
}

