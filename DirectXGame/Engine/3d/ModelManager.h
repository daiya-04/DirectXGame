#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <string>
#include <vector>
#include <array>
#include <memory>
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Matrix44.h"

class Model {
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
		float shininess_;
	};

private:

	//リソースの生成
	static ComPtr<ID3D12Resource> CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes);

public:

	static void SetDevice();

	void CreateBuffer();

	void SetVertexBuffers(ID3D12GraphicsCommandList* commandList);

	void SetGraphicsRootConstantBufferView(ID3D12GraphicsCommandList* commandList, UINT rootParamIndex);

	void SetColor(const Vector4& color) { materialData->color_ = color; }

	uint32_t GetUvHandle() const { return uvHandle_; }

	UINT GetVertices() const { return (UINT)vertices_.size(); }

public:

	std::vector<VertexData> vertices_;
	//uv
	int32_t uvHandle_ = 0;
	//ライティングの有無
	bool isLighting_ = true;
	//modelファイルの名前
	std::string name_;

private:

	static ID3D12Device* device_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	ComPtr<ID3D12Resource> vertexResource_;
	VertexData* vertexData = nullptr;
	
	ComPtr<ID3D12Resource> materialResource_;
	Material* materialData = nullptr;

};

class ModelManager{
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:

	static const size_t kNumModel = 124;

private:

	
	std::string filename_;
	std::string directoryPath_;
	std::vector<std::unique_ptr<Model>> models_;
	uint32_t useModelNum_= 0;

public:

	static ModelManager* GetInstance();

	static Model* Load(const std::string& modelName, bool isLighting = true);

private:

	Model* LoadInternal(const std::string& modelName, bool isLighting = true);

	void LoadObjFile(const std::string& modelName);

	void LoadMaterialTemplateFile(const std::string& fileName);

private:

	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(const ModelManager&) = delete;
	ModelManager& operator=(const ModelManager&) = delete;

};

