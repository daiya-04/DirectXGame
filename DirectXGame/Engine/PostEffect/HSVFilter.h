#pragma once
///---------------------------------------------------------------------------------------------
//
//　ポストエフェクト
// 画面の色をHSVで設定出来る
//
///---------------------------------------------------------------------------------------------

#include <d3d12.h>
#include <wrl.h>
#include "Vec2.h"
#include "Vec4.h"
#include "Matrix44.h"
#include <string>

//HSV調整項目
struct HSVFactor {
	//色相
	float hue;
	//彩度
	float saturation;
	//明度
	float value;
};

//HSVフィルタークラス
class HSVFilter {
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public: //メンバ関数
	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static HSVFilter* GetInstance();
	/// <summary>
	/// コンストラクタ
	/// </summary>
	HSVFilter();
	/// <summary>
	/// 初期化
	/// </summary>
	void Init();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
	/// <summary>
	/// 描画前
	/// </summary>
	void PreDrawScene();
	/// <summary>
	/// 描画後
	/// </summary>
	void PostDrawScene();
	/// <summary>
	/// デバッグ
	/// </summary>
	void DebugGUI();

private:
	/// <summary>
	/// パイプラインステート
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
	//
	static const float kClearColor_[4];
	//パイプラインステート
	ComPtr<ID3D12PipelineState> piplineState_;
	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature_;

	//テクスチャリソース
	ComPtr<ID3D12Resource> texBuff_;

	//srvハンドル
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> srvHandle_;

	//dsvハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandleCPU_{};
	//rtvハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandleCPU_{};

	//調整項目リソース
	ComPtr<ID3D12Resource> factorBuff_;
	//調整項目データ
	HSVFactor* factorData_;

};

