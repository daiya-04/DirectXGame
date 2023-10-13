#pragma once
#include <d3d12.h>
#include <dxcapi.h>
#include <wrl.h>
#include <string>
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"
//#include "Model.h"
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Matrix44.h"
#include <vector>

class Object3d{
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:

	struct VertexData {
		Vector4 pos_;
		Vector2 uv_;
		Vector3 normal;
	};

	struct Material {
		Vector4 color_;
		int32_t enableLightnig_;
		float padding_[3];
		Matrix4x4 uvtransform_;
	};

	struct MaterialData {
		std::string textureFilePath_;
	};

	struct ModelData {
		std::vector<VertexData> vertices_;
		MaterialData material_;
	};
	struct TransformationMatrix {
		Matrix4x4 WVP;
		Matrix4x4 World;
	};
	struct DirectionalLight {
		Vector4 color;
		Vector3 direction;
		float intensity;
	};

private: //静的メンバ変数

	static ID3D12Device* device_;
	static ID3D12GraphicsCommandList* commandList_;
	static ComPtr<ID3D12RootSignature> rootSignature_;
	static ComPtr<ID3D12PipelineState> graphicsPipelineState_;
	static Matrix4x4 viewMat_;
	static Matrix4x4 projectionMat_;
	
	static ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_;
	static UINT srvDescriptorHandleSize_;
	static ComPtr<ID3D12Resource> textureResource_;
	static ComPtr<ID3D12Resource> intermediateResource_;
	static D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU_;
	static D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_;
	static Matrix4x4 cameraMat_;

public: //静的メンバ関数

	static void StaticInitialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, int windowWidth, int windowHeight);

	static Object3d* Create(const std::string& filePath);

	static void preDraw();

	static void postDraw();

	static void SetCamera(const Vector3& rotate, const Vector3& position);

	static void Finalize();

	
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


	//ログ
	static void Log(const std::string& message);

	static std::wstring ConvertString(const std::string& str);

	static std::string ConvertString(const std::wstring& str);

	static ComPtr<ID3D12Resource> CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes);

	
private: //メンバ変数

	//Model* model_ = nullptr;
	const std::string directoryPath_ = "Resources";
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	ComPtr<ID3D12Resource> vertexResource_;
	ComPtr<ID3D12Resource> materialResource_;
	ComPtr<ID3D12Resource> wvpResource_;
	ComPtr<ID3D12Resource> directionalLightResource_;
	UINT index_ = 0;

	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };
	Vector3 position_{};
	Vector3 size_ = { 1.0f,1.0f,1.0f };
	Vector3 rotate_{};
	

public: //メンバ関数

	void Initialize(const std::string& filePath);

	void Draw();

private:

	ModelData LoadObjFile(const std::string& filename);

	MaterialData LoadMaterialTemplateFile(const std::string& filename);

	void LoadTexture(const std::string& filePath);

	[[nodiscard]]
	ComPtr<ID3D12Resource> UploadTextureData(const DirectX::ScratchImage& mipImage);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ComPtr<ID3D12DescriptorHeap> descriptorHeap, UINT descriptorSize, UINT index);

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ComPtr<ID3D12DescriptorHeap> descriptorHeap, UINT descriptorSize, UINT index);

};

