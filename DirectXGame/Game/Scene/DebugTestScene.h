#pragma once
///---------------------------------------------------------------------------------------------
// 
// デバッグテストシーン
// 基本的にエンジンの描画とかの機能テスト用
// ゲーム中には呼び出されないし、呼び出さない
// 今はパーティクルエディタでエフェクトを作ってるけど
// いずれ別でエディタ用のシーンを作成する予定
// 
///---------------------------------------------------------------------------------------------

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

//デバッグテストシーンクラス
class DebugTestScene : public IScene {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Init()override;
	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;
	/// <summary>
	/// 背景描画
	/// </summary>
	void DrawBackGround()override;
	/// <summary>
	/// モデル描画
	/// </summary>
	void DrawModel()override;
	/// <summary>
	/// パーティクル3dモデル描画
	/// </summary>
	void DrawParticleModel()override;
	/// <summary>
	/// パーティクル描画
	/// </summary>
	void DrawParticle()override;
	/// <summary>
	/// UI描画
	/// </summary>
	void DrawUI()override;
	/// <summary>
	/// ポストエフェクトを掛けるモデルなどの描画
	/// </summary>
	void DrawPostEffect()override;
	/// <summary>
	/// レンダーテクスチャの描画
	/// </summary>
	void DrawRenderTexture()override;
	/// <summary>
	/// デバッグ用
	/// </summary>
	void DebugGUI()override;
	/// <summary>
	/// デストラクタ
	/// </summary>
	~DebugTestScene()override {}

private: //必須項目
	//カメラ
	Camera camera_;
	//ポイントライト
	PointLight pointLight_;
	//スポットライト
	SpotLight spotLight_;

private: //テスト用の変数

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

