#pragma once
///---------------------------------------------------------------------------------------------
//
//　ポストエフェクト
// 今は死亡演出用のポストエフェクト
//
///---------------------------------------------------------------------------------------------

#include <d3d12.h>
#include <wrl.h>
#include "Vec2.h"
#include "Vec4.h"
#include "Matrix44.h"
#include <string>
#include <array>

//死亡演出に必用なデータ
struct DeadEffectData {
	//演出中か
	int32_t isEffect_;
	//補間用媒介変数表示
	float param_;
	//指数
	float root_;
	//輝度値
	float brightness_;
};

//ポストエフェクトクラス
class PostEffect {
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public: //メンバ関数
	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static PostEffect* GetInstance();
	/// <summary>
	/// コンストラクタ
	/// </summary>
	PostEffect();
	/// <summary>
	/// 初期化
	/// </summary>
	void Init();
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	void Draw(ID3D12GraphicsCommandList* cmdList);
	/// <summary>
	/// 描画前
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	void PreDrawScene(ID3D12GraphicsCommandList* cmdList);
	/// <summary>
	/// 描画後
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	void PostDrawScene(ID3D12GraphicsCommandList* cmdList);
	/// <summary>
	/// 演出のオン・オフ
	/// </summary>
	/// <param name="isGrayScale">演出をするかしないか</param>
	void SetGrayScaleEffect(bool isGrayScale) { deadEffectData_->isEffect_ = isGrayScale; }

private:
	/// <summary>
	/// パイプラインステート生成
	/// </summary>
	void CreateGraphicsPipelineState();
	/// <summary>
	/// リソース生成
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="sizeInBytes">リソースサイズ</param>
	/// <returns></returns>
	ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

private: //メンバ変数
	//クリアカラー
	static const float kClearColor_[4];
	//パイプラインステート
	ComPtr<ID3D12PipelineState> piplineState_;
	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature_;
	//テクスチャリソース
	ComPtr<ID3D12Resource> texBuff_;
	//srvハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU_{};
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_{};

	//dsvハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandleCPU_{};
	//rtvハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandleCPU_{};

	//演出リソース
	ComPtr<ID3D12Resource> grayScaleBuffer_;
	//演出データ
	DeadEffectData* deadEffectData_ = nullptr;

};

