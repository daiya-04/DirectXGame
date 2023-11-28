#pragma once

#include "../IScene.h"

class SelectScene final : public IScene
{
public:
	// 初期化
	void Initialize() override;

	void Reset()  override;

	// 更新
	void Update() override;

	// モデルの描画
	void DrawModel() override;
	//　UI の描画
	void DrawUI() override;

	// デストラクタ
	~SelectScene() override;

	void DebugGUI() override;


private: // プライベート関数



public:

	static const size_t stageNum_ = 8;

private: // プライベート変数

	std::array<Vector2, stageNum_> stageNumPos_;

	std::array<std::unique_ptr<Sprite>, stageNum_> number_;
	std::unique_ptr<Sprite> header_;
	std::unique_ptr<Sprite> stageImgFrame_;

};