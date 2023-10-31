#pragma once

#include "../IScene.h"
#include "../../Object/ObjectList.h"

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

	std::unique_ptr<Player> player_;
	std::unique_ptr<ViewProjection> viewProjection_;

};