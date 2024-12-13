#pragma once

class SceneManager;

//シーンの抽象クラス
class IScene{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init() = 0;
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;
	/// <summary>
	/// モデル描画
	/// </summary>
	virtual void DrawModel() = 0;
	/// <summary>
	/// 背景描画
	/// </summary>
	virtual void DrawBackGround() = 0;
	/// <summary>
	/// UI描画
	/// </summary>
	virtual void DrawUI() = 0;
	/// <summary>
	/// パーティクル描画
	/// </summary>
	virtual void DrawParticle() = 0;
	/// <summary>
	/// モデルパーティクル描画
	/// </summary>
	virtual void DrawParticleModel() = 0;
	/// <summary>
	/// ポストエフェクトを掛けるやつ描画
	/// </summary>
	virtual void DrawPostEffect() = 0;
	/// <summary>
	/// レンダーテクスチャ描画
	/// </summary>
	virtual void DrawRenderTexture() = 0;
	/// <summary>
	/// デバッグ
	/// </summary>
	virtual void DebugGUI() = 0;

	virtual ~IScene() {};

};

