#pragma once
#include "ISceneState.h"
#include "Sprite.h"

class BattleState : public ISceneState {
public:

	BattleState(GameScene* gameScene) : gameScene_(gameScene) {}

	~BattleState() override = default;

	void Init() override;

	void Update() override;

	void DrawUI() override;

private:

	GameScene* gameScene_ = nullptr;


};

