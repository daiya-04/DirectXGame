#pragma once
#include "ISceneState.h"
#include <cstdint>

class GameOverState : public ISceneState {
public:

	GameOverState(GameScene* gameScene) : gameScene_(gameScene) {}

	~GameOverState() override = default;

	void Init() override;

	void Update() override;

	void DrawUI() override;


private:

	GameScene* gameScene_ = nullptr;

	float alpha_ = 0.0f;
	float fadeSpeed_ = 0.01f;

	//戦闘が終了してからタイトルに戻るまでの時間
	uint32_t finishTime_ = 60 * 3;
	//ゲーム終了カウンター
	uint32_t finishCount_ = finishTime_;

};

