#include "GameOverState.h"
#include "GameScene.h"
#include "SceneManager.h"
#include "PostEffect.h"

#include <cassert>

void GameOverState::Init() {

	assert(gameScene_);

	alpha_ = 0.0f;
	PostEffect::GetInstance()->SetGrayScaleEffect(true);
	gameScene_->GetPlayer()->GetObj()->SetVisible(false);

}

void GameOverState::Update() {

	alpha_ += fadeSpeed_;
	alpha_ = std::min(alpha_, 1.0f);

	if (alpha_ >= 1.0f) {
		finishCount_--;
	}

	if (finishCount_ <= 0) {
		DaiEngine::SceneManager::GetInstance()->ChangeScene("Title");
	}

	gameScene_->GetGameOverUI()->SetColor({ 1.0f,1.0f,1.0f,alpha_ });
}

void GameOverState::DrawUI() {
	gameScene_->GetGameOverUI()->Draw();
}
