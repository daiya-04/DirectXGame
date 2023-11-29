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
	// Particle の描画
	void DrawParticle() override;

	// デストラクタ
	~TitleScene() override;

	void DebugGUI() override;


private: // プライベート関数



private: // プライベート変数

	
	std::unique_ptr<Sprite> title_;
	std::unique_ptr<Sprite> pushSpace_;

	//std::unique_ptr<ViewProjection> viewProjection_;

	
};