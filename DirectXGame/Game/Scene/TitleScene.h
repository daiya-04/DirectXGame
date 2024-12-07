#pragma once
#include "IScene.h"
#include <memory>
#include <list>
#include <map>
#include <string>

#include "Sprite.h"
#include "Object3d.h"
#include "Particle.h"
#include "Camera.h"
#include "WorldTransform.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Vec2.h"
#include "GPUParticle.h"

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
	//調整項目の設定
	void SetGlobalVariables();
	//調整項目の代入
	void ApplyGlobalVariables();
	//UIエフェクト初期化
	void UIEffectInit();
	//UIエフェクト更新
	void UIEffectUpdate();
	//
	void WarpTransitionInit();
	//
	void WarpTransitionUpdate();

private:

	Camera camera_;
	PointLight pointLight_;
	SpotLight spotLight_;

	std::unique_ptr<Sprite> backGround_;

	std::unique_ptr<Sprite> Abutton_;
	std::unique_ptr<Sprite> AbuttonEff_;

	std::map<std::string, std::unique_ptr<GPUParticle>> titleEff_;
	std::map<std::string, std::unique_ptr<GPUParticle>> warpHole_;
	Vector3 warpHolePos_ = {};

	std::unique_ptr<Object3d> titleText_;

	std::unique_ptr<Sprite> fadeSprite_;

private://パラメータまとめたやつら

	struct WorkButtonEffect {
		float cycle_ = 0.0f;
		float alpha_ = 1.0f;
		float startScale_ = 0.7f;
		float endScale_ = 1.2f;
		float speed_ = 0.04f;
	};

	WorkButtonEffect AbuttonEffectParam_;

	struct WorkWarpTransition {
		bool isTransition_ = false;
		float alpha_ = 0.0f;
		Vector3 startCameraPos_{};
		Vector3 endCameraPos_{};
		float speed_ = 0.01f;
		float param_ = 0.0f;
	};

	WorkWarpTransition warpTransitionParam_;

};

