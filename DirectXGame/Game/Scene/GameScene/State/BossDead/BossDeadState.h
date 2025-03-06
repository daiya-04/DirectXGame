#pragma once
#include "ISceneState.h"
#include "Vec3.h"
#include "Matrix44.h"

class BossDeadState : public ISceneState {
public:

	BossDeadState(GameScene* gameScene) : gameScene_(gameScene) {}

	~BossDeadState() override = default;

	void Init() override;

	void Update() override;

	void DrawUI() override {}

	void SetCamera();

private:

	GameScene* gameScene_ = nullptr;

	//死亡アニメーションが終わってからの間の時間
	int32_t interval_ = 30;
	//死亡アニメーションが終わってからの時間
	int32_t count_ = 0;
	//カメラの位置
	Vector3 cameraPos_ = {};
	//カメラの回転行列
	Matrix4x4 rotateMat_ = MakeIdentity44();
	//ターゲットからのオフセット
	Vector3 offset_ = { -3.0f,2.0f,-8.0f };

};

