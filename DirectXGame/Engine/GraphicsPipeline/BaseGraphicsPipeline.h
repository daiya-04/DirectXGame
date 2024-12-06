#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "DXCompiler.h"

class BaseGraphicsPipeline {
protected:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:

	virtual void Init();

	virtual void preDraw();

	void preDrawScreen();

protected:

	ID3D12Device* device_ = nullptr;
	DXCompiler* dxCompiler_ = nullptr;
	ComPtr<ID3D12RootSignature> rootSignature_;
	ComPtr<ID3D12PipelineState> pipelineState_;
	D3D12_PRIMITIVE_TOPOLOGY primitiveTopology_ = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ComPtr<ID3D12RootSignature> screenRSig_;
	ComPtr<ID3D12PipelineState> screenPS_;

};

