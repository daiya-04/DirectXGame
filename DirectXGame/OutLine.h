#pragma once
#include <d3d12.h>
#include <dxcapi.h>
#include <wrl.h>
#include "Vec2.h"
#include "Vec4.h"
#include "Matrix44.h"
#include <string>
#include <array>

class OutLine {
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public: //メンバ関数

	static OutLine* GetInstance();

	OutLine();

	void Init();

	void Draw(ID3D12GraphicsCommandList* cmdList);

	void PreDrawScene(ID3D12GraphicsCommandList* cmdList);

	void PostDrawScene(ID3D12GraphicsCommandList* cmdList);

private:

	void CreateGraphicsPipelineState();

	ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

	ComPtr<IDxcBlob> CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandleer);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ComPtr<ID3D12DescriptorHeap> descriptorHeap, UINT descriptorSize, UINT index);

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ComPtr<ID3D12DescriptorHeap> descriptorHeap, UINT descriptorSize, UINT index);

private: //メンバ変数

	static const float clearColor_[4];

	ComPtr<ID3D12PipelineState> piplineState_;
	ComPtr<ID3D12RootSignature> rootSignature_;

	ComPtr<ID3D12Resource> texBuff_;

	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU_{};
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_{};

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandleCPU_{};
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandleCPU_{};

};

