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

	void SetStageNum(int num) override { stageNum_ = num; }

private: // プライベート関数



private: // プライベート変数


	// ゲームシーン内でデータを保存する
	std::unique_ptr<ViewProjection> maingCamera_;

	int stageNum_ = 0;

	// ステージを持たせる
	std::unique_ptr<DebugStage> currentStage_;

	// 原点へのオフセット
	Vector3 kOriginOffset_ = { 0.0f,17.0f,-10.0f };
	Vector3 kOriginAngle = { 1.0f,0.0f,0.0f, };

};