#pragma once
#include "ISceneState.h"
#include <cstdint>

class ClearState : public ISceneState {
public:

	ClearState(GameScene* gameScene) : gameScene_(gameScene) {}

	~ClearState() override = default;

	void Init() override;

	void Update() override;

	void DrawUI() override;

private:

	GameScene* gameScene_ = nullptr;

	//戦闘が終了してからタイトルに戻るまでの時間
	uint32_t finishTime_ = 60 * 3;
	//ゲーム終了カウンター
	uint32_t finishCount_ = finishTime_;

};

