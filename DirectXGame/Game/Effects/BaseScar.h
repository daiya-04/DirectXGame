#pragma once
///---------------------------------------------------------------------------------------------
// 
// 攻撃が地面に着弾したときに出す「跡」の基底クラス
// 
///---------------------------------------------------------------------------------------------

#include <d3d12.h>
#include <wrl.h>
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Matrix44.h"
#include "WorldTransform.h"
#include "Camera.h"

//跡の基底クラス
class BaseScar {
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
protected:
	//ルートパラメータ
	enum class RootParameter {
		kWorldTransform,
		kCamera,
		kTexture,
		kMaskTex,
		kScarData,

		kParamNum,
	};
	//頂点データ
	struct VertexData {
		//座標
		Vector4 pos_;
		//UV座標
		Vector2 uv_;
	};
	//Cbuffer用
	struct ScarData {
		//色
		Vector4 color_;
		//閾値
		float threshold_;
	};

protected: //静的メンバ変数
	//デバイス
	static ID3D12Device* device_;
	//コマンドリスト
	static ID3D12GraphicsCommandList* commandList_;

protected: //グラフィックリソース
	//ワールド行列バッファ
	ComPtr<ID3D12Resource> worldMatBuff_;
	//ワールド行列バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	//頂点バッファ
	ComPtr<ID3D12Resource> vertexBuff_;
	//インデックスバッファ
	ComPtr<ID3D12Resource> indexBuff_;
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	//固有データバッファ
	ComPtr<ID3D12Resource> scarDataBuff_;
	//固有データ
	ScarData* scarData_ = nullptr;
	//テクスチャハンドル
	uint32_t textureHandle_ = 0;
	//マスク画像ハンドル
	uint32_t maskTex_ = 0;

protected: //オブジェクトパラメータ

	//座標
	Vector3 position_{};
	//倍率
	Vector2 scale_ = { 2.0f,2.0f };
	//回転
	float rotate_{};
	//ディゾルブ用閾値
	float threshold_ = 0.0f;
	//色
	Vector4 color_ = {};
	//演出中か
	bool isEffect_ = false;
	//演出時間
	int32_t lifeTime_ = 60 * 6;
	//演出タイマー
	int32_t lifeTimer_ = lifeTime_;

protected:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="textureHandle">テクスチャハンドル</param>
	virtual void Init(uint32_t textureHandle);
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();
	/// <summary>
	/// エフェクト更新
	/// </summary>
	virtual void EffectUpdate() {};
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	virtual void Draw(const Camera& camera);
	/// <summary>
	/// 演出開始
	/// </summary>
	/// <param name="pos">中心座標</param>
	virtual void EffectStart(const Vector3& pos);

public:
	/// <summary>
	/// 高さ調整
	/// </summary>
	/// <param name="height">高さ</param>
	void HeightAdjustment(float height);
	/// <summary>
	/// 演出中かどうか
	/// </summary>
	/// <returns>演出中はtrue、それ以外はfalse</returns>
	bool IsEffect() const { return isEffect_; }

private:
	/// <summary>
	/// 頂点情報の設定
	/// </summary>
	void TransferVertex();
	/// <summary>
	/// バッファの生成
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="sizeInBytes">リソースのサイズ</param>
	/// <returns>生成されたバッファリソース</returns>
	ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

};

