#pragma once
#include <d3d12.h>
#include <dxcapi.h>
#include <wrl.h>
#include "Vec2.h"
#include "Vec4.h"
#include "Matrix44.h"
#include <string>

struct HSVFactor {
	float hue;
	float saturation;
	float value;
};

class HSVFilter {
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public: //メンバ関数

	static HSVFilter* GetInstance();

	HSVFilter();

	void Init();

	void Draw();

	void PreDrawScene();

	void PostDrawScene();

	void DebugGUI();

private:

	void CreateGraphicsPipelineState();

	ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

	ComPtr<IDxcBlob> CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandleer);

private: //メンバ変数

	static const float clearColor_[4];

	ComPtr<ID3D12PipelineState> piplineState_;
	ComPtr<ID3D12RootSignature> rootSignature_;

	ComPtr<ID3D12Resource> texBuff_;

	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> srvHandle_;

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandleCPU_{};
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandleCPU_{};

	ComPtr<ID3D12Resource> factorBuff_;
	HSVFactor* factorData_;

};

