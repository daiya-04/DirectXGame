#include "ClearState.h"
#include "GameScene.h"
#include "SceneManager.h"

#include <cassert>


void ClearState::Init() {

	assert(gameScene_);

}

void ClearState::Update() {

	if (--finishCount_ <= 0) {
		DaiEngine::SceneManager::GetInstance()->ChangeScene("Title");
	}

	gameScene_->GetPlayer()->Update();
	gameScene_->GetFollowCamera()->Update();

	gameScene_->GetCamera().SetMatView(gameScene_->GetFollowCamera()->GetCamera().GetMatView());
}

void ClearState::DrawUI() {
	gameScene_->GetFinishUI()->Draw();
}

