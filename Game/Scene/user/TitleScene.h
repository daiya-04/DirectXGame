#pragma once

#include "../IScene.h"

class TitleScene final : public IScene
{
public:
	// 初期化
	void Initialize() override;

	void Reset() override;

	// 更新
	void Update() override;

	// モデルの描画
	void DrawModel() override;
	// UI の描画
	void DrawUI() override;

	// デストラクタ
	~TitleScene() override;

	void DebugGUI() override;


private: // プライベート関数



private: // プライベート変数

	std::unique_ptr<Sprite> sprite_;
	float rotate_;
	Vector2 pos_;

	std::unique_ptr<Object3d> pot_;
	std::unique_ptr<Object3d> plane_;

	std::unique_ptr<ViewProjection> viewProjection_;

	WorldTransform wt_;
	WorldTransform wtPlane_;
};