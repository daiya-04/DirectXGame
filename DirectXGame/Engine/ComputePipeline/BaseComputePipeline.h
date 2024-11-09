#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "DXCompiler.h"


class BaseComputePipeline {
protected:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:

	virtual void Init();

	virtual void preDispatch();

protected:

	ID3D12Device* device_ = nullptr;
	DXCompiler* dxCompiler_ = nullptr;
	ComPtr<ID3D12RootSignature> rootSignature_;
	ComPtr<ID3D12PipelineState> pipelineState_;

};

