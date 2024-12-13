#pragma once
///---------------------------------------------------------------------------------------------
//
// BaseComputePipeline
// コンピュートパイプラインステート生成の基底クラス
//
///---------------------------------------------------------------------------------------------

#include <d3d12.h>
#include <wrl.h>
#include "DXCompiler.h"


class BaseComputePipeline {
protected:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init();
	/// <summary>
	/// ディスパッチ起動前
	/// </summary>
	virtual void preDispatch();

protected:

	ID3D12Device* device_ = nullptr;
	DXCompiler* dxCompiler_ = nullptr;
	ComPtr<ID3D12RootSignature> rootSignature_;
	ComPtr<ID3D12PipelineState> pipelineState_;

};

