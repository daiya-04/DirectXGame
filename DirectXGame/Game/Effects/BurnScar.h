#pragma once
///---------------------------------------------------------------------------------------------
// 
// 焼け跡
// 火の玉が着弾したときに地面に着く焼け跡
// 
///---------------------------------------------------------------------------------------------

#include <d3d12.h>
#include <dxcapi.h>
#include <wrl.h>
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Matrix44.h"
#include "WorldTransform.h"
#include "Camera.h"
#include "GPUParticle.h"
#include "BaseScar.h"

#include <string>
#include <functional>
#include <map>
#include <optional>

//焼け跡クラス
class BurnScar : public BaseScar {
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private: //静的メンバ変数
	//ルートシグネチャ
	static ComPtr<ID3D12RootSignature> rootSignature_;
	//パイプラインステート
	static ComPtr<ID3D12PipelineState> graphicsPipelineState_;

public: //静的メンバ関数

	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="commandList">コマンドライン</param>
	static void StaticInit(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	/// <summary>
	/// モデルの生成
	/// </summary>
	/// <param name="textureHandle">テクスチャハンドル</param>
	/// <returns>新しいオブジェクト</returns>
	static BurnScar* Create(uint32_t textureHandle);
	/// <summary>
	/// 描画前処理
	/// </summary>
	static void preDraw();

private: //メンバ変数

	///エフェクト
	//着弾時
	std::map<std::string, std::unique_ptr<GPUParticle>> impact_;
	//跡から出てくる火の粉
	std::map<std::string, std::unique_ptr<GPUParticle>> residual_;

public: //メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="textureHandle">テクスチャハンドル</param>
	void Init(uint32_t textureHandle) override;
	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;
	/// <summary>
	/// エフェクト更新
	/// </summary>
	void EffectUpdate() override;
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(const Camera& camera) override;
	/// <summary>
	/// パーティクル描画
	/// </summary>
	/// <param name="camera"></param>
	void DrawParticle(const Camera& camera);
	/// <summary>
	/// 演出開始
	/// </summary>
	/// <param name="pos">中心座標</param>
	void EffectStart(const Vector3& pos)override;

};

