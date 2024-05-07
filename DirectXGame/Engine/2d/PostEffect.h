#pragma once
#include <d3d12.h>
#include <dxcapi.h>
#include <wrl.h>
#include "Vec2.h"
#include "Vec4.h"
#include "Matrix44.h"
#include <string>
#include <array>

struct GrayScale {
	int32_t isGrayScale_;
	float param_;
};

class PostEffect {
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:

	struct VertexData {
		Vector4 pos_;
		Vector2 uv_;
	};

	struct MaterialData {
		Vector4 color_;
	};

	

public: //メンバ関数

	static PostEffect* GetInstance();

	PostEffect();

	void Init();

	void Draw(ID3D12GraphicsCommandList* cmdList);

	void PreDrawScene(ID3D12GraphicsCommandList* cmdList);

	void PostDrawScene(ID3D12GraphicsCommandList* cmdList);

	void SetGrayScaleEffect(bool isGrayScale) { grayScaleData_->isGrayScale_ = isGrayScale; }

private:

	void TransferVertex();

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
	ComPtr<ID3D12Resource> depthBuff_;

	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU_{};
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_{};

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandleCPU_{};
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandleCPU_{};

	ComPtr<ID3D12Resource> vertexBuff_;
	VertexData* vertexData_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	ComPtr<ID3D12Resource> indexBuff_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	ComPtr<ID3D12Resource> materialBuff_;
	MaterialData* materialData_ = nullptr;

	ComPtr<ID3D12Resource> grayScaleBuffer_;
	GrayScale* grayScaleData_ = nullptr;

};

