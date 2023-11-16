#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <string>
#include <vector>
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Matrix44.h"

class Model{
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
		
		MaterialData material_;
	};

private: //静的メンバ変数

	static ID3D12Device* device_;

public: //静的メンバ関数

	static Model* LoadOBJ(const std::string& modelName);

	static void SetDevice(ID3D12Device* device) { device_ = device; }

private:

	//リソースの生成
	static ComPtr<ID3D12Resource> CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes);

private: //メンバ変数

	
	std::string filename_;
	std::string directoryPath_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	ComPtr<ID3D12Resource> vertexResource_;
	std::vector<VertexData> vertices_;
	ComPtr<ID3D12Resource> materialResource_;
	UINT index_ = 0;
	int32_t uvHandle_ = 0;

	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };

public: //メンバ関数

	void Draw(ID3D12GraphicsCommandList* commandList, UINT rootParamIndexMaterial);

private:

	void LoadObjFile(const std::string& modelName);

	void LoadMaterialTemplateFile(const std::string& fileName);

	void CreateBuffer();

};

