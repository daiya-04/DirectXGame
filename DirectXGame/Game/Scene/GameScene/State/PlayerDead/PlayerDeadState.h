#pragma once
#include "ISceneState.h"
#include "Vec3.h"
#include "Player.h"
#include "Matrix44.h"

class PlayerDeadState : public ISceneState {
public:

	PlayerDeadState(GameScene* gameScene) : gameScene_(gameScene) {}

	~PlayerDeadState() override = default;

	void Init() override;

	void Update() override;

	void DrawUI() override {}

	void SetCamera();

private:

	GameScene* gameScene_ = nullptr;

	Player* player_ = nullptr;

	//死亡アニメーションが終わってからの間
	int32_t interval_ = 30;
	//死亡アニメーションが終わってからの時間
	int32_t count_ = 0;
	//カメラの位置
	Vector3 cameraPos_ = {};
	//カメラの回転角
	Vector3 cameraRotate_ = { -0.14f,-2.9f,0.0f };
	//ターゲットからのオフセット
	Vector3 offset_ = { 1.5f, -0.5f, 6.0f };


};

