#pragma once

#include "../IScene.h"

class GameScene final : public IScene
{
public:
	// 初期化
	void Initialize() override;

	// 更新
	void Update() override;

	// モデルの描画
	void DrawModel() override;
	// UI の描画
	void DrawUI() override;


	// デストラクタ
	~GameScene() override;

private: // プライベート関数



private: // プライベート変数


};