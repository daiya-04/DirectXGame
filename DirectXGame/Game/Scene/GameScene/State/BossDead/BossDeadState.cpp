#include "BossDeadState.h"
#include "GameScene.h"

#include <cassert>

void BossDeadState::Init() {

	assert(gameScene_);

	count_ = 0;

	SetCamera();

	gameScene_->GetPlayer()->GetObj()->SetVisible(false);

}

void BossDeadState::Update() {

	gameScene_->GetBoss()->Update();

	if (gameScene_->GetBoss()->IsFinishDeadMotion()) {
		if (++count_ >= interval_) {
			gameScene_->ChangeState(GameScene::SceneEvent::Clear);
		}
	}
}

void BossDeadState::SetCamera() {
	//カメラのターゲットの座標
	Vector3 targetPos = gameScene_->GetBoss()->GetCenterPos();
	//カメラの座標計算
	cameraPos_ = targetPos + offset_;
	//座標更新
	gameScene_->GetCamera().translation_ = cameraPos_;
	
	Vector3 sub = targetPos - cameraPos_;
	rotateMat_ = DirectionToDirection(Vector3(0.0f, 0.0f, 1.0f), sub);

	gameScene_->GetCamera().UpdateViewMatrixRotate(rotateMat_);

}
