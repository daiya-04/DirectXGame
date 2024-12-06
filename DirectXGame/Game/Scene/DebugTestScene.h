#pragma once
#include "IScene.h"
#include <memory>
#include <list>

#include "Sprite.h"
#include "Object3d.h"
#include "SkinningObject.h"
#include "SkinCluster.h"
#include "Particle.h"
#include "Camera.h"
#include "WorldTransform.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "SkyBox.h"
#include "BurnScar.h"
#include "IceScar.h"
#include "GPUParticle.h"

#include "Dissolve.h"
#include "PostEffect.h"

class DebugTestScene : public IScene {
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

	~DebugTestScene()override {}

private:

	Camera camera_;
	PointLight pointLight_;
	SpotLight spotLight_;

	std::unique_ptr<SkinningObject> human_;
	std::shared_ptr<Model> humanModel_;
	std::shared_ptr<Model> standingModel_;
	std::shared_ptr<Model> sneakModel_;
	Animation animation_;
	Skeleton skeleton_;
	SkinCluster skinCluster_;

	uint32_t skyBoxTex_ = 0;
	std::unique_ptr<SkyBox> skyBox_;

	std::shared_ptr<Model> model_;
	std::unique_ptr<Object3d> obj_;

	Dissolve* dissolve_;
	PostEffect* postEffect_ = nullptr;

	uint32_t tex_ = 0;
	std::unique_ptr<Sprite> sprite_;

	uint32_t burnScarsTex_ = 0;
	std::unique_ptr<BurnScar> burnScars_;
	std::unique_ptr<IceScar> iceScar_;

	std::map<std::string, std::unique_ptr<GPUParticle>> effect_;

};

