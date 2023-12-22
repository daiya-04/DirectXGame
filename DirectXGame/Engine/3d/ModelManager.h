#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <string>
#include <vector>
#include <array>
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Matrix44.h"

class ModelManager{
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:

	static const size_t kNumModel = 124;

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
		float shininess_;
	};

	struct Model {
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
		ComPtr<ID3D12Resource> vertexResource_;
		std::vector<VertexData> vertices_;
		ComPtr<ID3D12Resource> materialResource_;
		UINT index_ = 0;
		int32_t uvHandle_ = 0;
		std::string name_;
		bool isLighting_ = true;
	};

private:

	ID3D12Device* device_ = nullptr;
	std::string filename_;
	std::string directoryPath_;
	std::array<Model, kNumModel> models_;
	uint32_t useModelNum_= 0;

public:

	static ModelManager* GetInstance();

	static uint32_t Load(const std::string& modelName,bool isLighting = true);

	//リソースの生成
	static ComPtr<ID3D12Resource> CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes);

	void Initialize();

	void SetVertexBuffers(ID3D12GraphicsCommandList* commandList, uint32_t modelHandle);

	void SetGraphicsRootConstantBufferView(ID3D12GraphicsCommandList* commandList, UINT rootParamIndex, uint32_t modelHandle);

	uint32_t GetUvHandle(uint32_t modelHandle) { return models_[modelHandle].uvHandle_; }

	UINT GetIndex(uint32_t modelHandle) { return models_[modelHandle].index_; }

private:

	uint32_t LoadInternal(const std::string& modelName, bool isLighting);

	void LoadObjFile(const std::string& modelName);

	void LoadMaterialTemplateFile(const std::string& fileName);

	void CreateBuffer();


private:

	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(const ModelManager&) = delete;
	ModelManager& operator=(const ModelManager&) = delete;

};

