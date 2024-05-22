#pragma once
#include <d3d12.h>
#include <dxcapi.h>
#include <wrl.h>
#include <string>
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Matrix44.h"
#include "Camera.h"

class Line{
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
	
	struct DataForGPU {
		Matrix4x4 wvp_;
		Vector4 color_;
	};

private:

	//共通部分の変数
	static ID3D12Device* device_;
	static ID3D12GraphicsCommandList* commandList_;
	static ComPtr<ID3D12RootSignature> rootSignature_;
	static ComPtr<ID3D12PipelineState> graphicsPipelineState_;

public:

	//共通部分の関数

	static void Init(ID3D12Device* device);

	static void preDraw(ID3D12GraphicsCommandList* commandList);

	static void postDraw() {}

	static void AllDraw();

	static void Draw(const Vector3& start, const Vector3& end, const Camera& camera, const Vector4& color = {1.0f,1.0f,1.0f,1.0f});

	

private:
	/// <summary>
	/// シェーダーのコンパイル
	/// </summary>
	/// <param name="filePath">CompilerするShaderファイルへのパス</param>
	/// <param name="profile">Compilerに使用するProfile</param>
	/// <param name="dxcUtils"></param>
	/// <param name="dxcCompiler"></param>
	/// <param name="includeHandleer"></param>
	/// <returns></returns>
	static ComPtr<IDxcBlob> CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandleer);

	static void CreateGPS();
	//リソースの生成
	static ComPtr<ID3D12Resource> CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes);

	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ComPtr<ID3D12DescriptorHeap> descriptorHeap, UINT descriptorSize, UINT index);

	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ComPtr<ID3D12DescriptorHeap> descriptorHeap, UINT descriptorSize, UINT index);

private:

	static UINT drawNum_;
	static const uint32_t kMaxDrawNum_;

	static DataForGPU* datas_;
	static ComPtr<ID3D12Resource> instancingResource_;
	static D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU_;
	static D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU_;


private:

	Line() {};

};

