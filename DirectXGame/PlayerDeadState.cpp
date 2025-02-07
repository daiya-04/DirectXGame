#include "PlayerDeadState.h"
#include "GameScene.h"

#include <cassert>


void PlayerDeadState::Init() {

	assert(gameScene_);

	count_ = 0;

	cameraPos_ = gameScene_->GetPlayer()->GetCenterPos() + offset_;
	gameScene_->GetCamera().translation_ = cameraPos_;
	gameScene_->GetCamera().rotation_ = cameraRotate_;

	gameScene_->GetCamera().UpdateViewMatrix();

}

void PlayerDeadState::Update() {

	gameScene_->GetPlayer()->Update();

	if (gameScene_->GetPlayer()->IsFinishDeadMotion()) {
		if (++count_ >= interval_) {
			gameScene_->ChangeState(GameScene::SceneEvent::GameOver);
		}
	}

}
