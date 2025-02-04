#pragma once
///---------------------------------------------------------------------------------------------
// 
// タイトルシーン
// 
///---------------------------------------------------------------------------------------------


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


//タイトルシーンクラス
class TitleScene : public DaiEngine::IScene {
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
	~TitleScene()override;

private:
	/// <summary>
	/// GlobalVariablesに項目の追加
	/// </summary>
	void SetGlobalVariables();
	/// <summary>
	/// GlobalVariablesから変数に代入
	/// </summary>
	void ApplyGlobalVariables();
	/// <summary>
	/// UIエフェクト初期化
	/// </summary>
	void UIEffectInit();
	/// <summary>
	/// UIエフェクト更新
	/// </summary>
	void UIEffectUpdate();
	/// <summary>
	/// ゲーム開始演出初期化
	/// </summary>
	void WarpTransitionInit();
	/// <summary>
	/// ゲーム開始演出更新
	/// </summary>
	void WarpTransitionUpdate();

private:
	//カメラ
	DaiEngine::Camera camera_;
	//ポイントライト
	DaiEngine::PointLight pointLight_;
	//スポットライト
	DaiEngine::SpotLight spotLight_;
	//背景
	std::unique_ptr<DaiEngine::Sprite> backGround_;
	//AボタンUI
	std::unique_ptr<DaiEngine::Sprite> Abutton_;
	//AボタンUIエフェクト
	std::unique_ptr<DaiEngine::Sprite> AbuttonEff_;
	//ワープホールまでの道
	std::map<std::string, std::unique_ptr<DaiEngine::GPUParticle>> runWay_;
	//道の座標
	Vector3 runWayPos_{ 0.0f,-1.0f,0.0f };
	//ワープホール
	std::map<std::string, std::unique_ptr<DaiEngine::GPUParticle>> warpHole_;
	//ワープホールの位置座標
	Vector3 warpHolePos_ = {};
	//タイトル文字(3D)
	std::unique_ptr<DaiEngine::Object3d> titleText_;
	//開始演出に使うフェード用スプライト
	std::unique_ptr<DaiEngine::Sprite> fadeSprite_;

private://パラメータまとめたやつら

	//ボタンのエフェクトに必要なパラメータ
	struct WorkButtonEffect {
		//サイクル
		float cycle_ = 0.0f;
		//α値
		float alpha_ = 1.0f;
		//開始スケール
		float startScale_ = 0.7f;
		//終了スケール
		float endScale_ = 1.2f;
		//演出スピード
		float speed_ = 0.04f;
	};
	//Aボタンエフェクトのパラメータ
	WorkButtonEffect AbuttonEffectParam_;

	//ゲーム開始演出に必要なパラメータ
	struct WorkWarpTransition {
		//演出中か
		bool isTransition_ = false;
		//α値
		float alpha_ = 0.0f;
		//カメラの開始座標
		Vector3 startCameraPos_{};
		//カメラの終了座標
		Vector3 endCameraPos_{};
		//カメラの移動スピード
		float speed_ = 0.01f;
		//移動イージングパラメータ
		float param_ = 0.0f;
	};
	//ゲーム開始演出のパラメータ
	WorkWarpTransition warpTransitionParam_;

};

