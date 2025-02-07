#include "BossDeadState.h"
#include "GameScene.h"

#include <cassert>

void BossDeadState::Init() {

	assert(gameScene_);

	count_ = 0;

	cameraPos_ = gameScene_->GetBoss()->GetCenterPos() + offset_;
	gameScene_->GetCamera().translation_ = cameraPos_;
	gameScene_->GetCamera().rotation_ = cameraRotate_;

	gameScene_->GetCamera().UpdateViewMatrix();

}

void BossDeadState::Update() {

	gameScene_->GetBoss()->Update();

	if (gameScene_->GetBoss()->IsFinishDeadMotion()) {
		if (++count_ >= interval_) {
			gameScene_->ChangeState(GameScene::SceneEvent::Clear);
		}
	}


}
