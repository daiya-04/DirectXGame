#pragma once

#include "../IScene.h"
#include "../../Stage/DebugStage.h"

class GameScene final : public IScene
{
public:
	// 初期化
	void Initialize() override;
	// シーン切り替え時の処理
	void Reset() override;

	// 更新
	void Update() override;

	// モデルの描画
	void DrawModel() override;
	// UI の描画
	void DrawUI() override;

	// デストラクタ
	~GameScene() override;

	void DebugGUI() override;

private: // プライベート関数



private: // プライベート変数


	// ゲームシーン内でデータを保存する
	std::unique_ptr<ViewProjection> viewProjection_;

	// ステージを持たせる
	std::unique_ptr<DebugStage> debugStage_;

};