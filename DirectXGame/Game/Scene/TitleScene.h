#pragma once
#include "IScene.h"
#include <memory>
#include <list>

#include "Sprite.h"
#include "Object3d.h"
#include "Particle.h"
#include "Camera.h"
#include "WorldTransform.h"

class TitleScene : public IScene {
public:
	//初期化
	void Init()override;
	//更新
	void Update()override;
	//背景描画
	void DrawBackGround()override;
	//モデル描画
	void DrawModel()override;
	//パーティクル3dモデル描画
	void DrawParticleModel()override;
	//パーティクル描画
	void DrawParticle()override;
	//UI描画
	void DrawUI()override;
	//ポストエフェクトを掛けるモデルなどの描画
	void DrawPostEffect()override;
	//レンダーテクスチャの描画
	void DrawRenderTexture()override;
	//デバッグ用
	void DebugGUI()override;

	~TitleScene()override;

private:

	std::unique_ptr<Sprite> title_;
	std::unique_ptr<Sprite> Abutton_;

	int count_ = 0;

};

