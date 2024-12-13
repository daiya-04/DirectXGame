#pragma once
///---------------------------------------------------------------------------------------------
//
// ポストエフェクト
// 画面にディゾルブ効果をつける
//
///---------------------------------------------------------------------------------------------


#include <d3d12.h>
#include <wrl.h>
#include "Vec2.h"
#include "Vec4.h"
#include "Matrix44.h"
#include <string>
#include <array>

//ディゾルブクラス
class Dissolve{
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
	//調整項目
	struct CBuffData {
		//閾値
		float threshold_;
	};

public: //メンバ関数
	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static Dissolve* GetInstance();
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Dissolve();
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
	/// デバッグ
	/// </summary>
	void DebugGUI();

private:
	/// <summary>
	/// パイプラインステート生成
	/// </summary>
	void CreateGraphicsPipelineState();
	/// <summary>
	/// リソース生成
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="sizeInBytes">リソースのサイズ</param>
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

	//調整項目リソース
	ComPtr<ID3D12Resource> buff_;
	//調整項目データ
	CBuffData* buffData_ = nullptr;

	//マスク画像テクスチャハンドル
	uint32_t maskTex_ = 0;

};

