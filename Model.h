#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <string>
#include <vector>
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"

class Model{
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:

	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};

	struct MaterialData {
		std::string textureFilePath;
	};

	struct ModelData {
		std::vector<VertexData> vertices;
		MaterialData material;
	};

private: //静的メンバ変数

	static ComPtr<ID3D12Resource> vertexResource_;

public: //静的メンバ関数

	static Model* LoadOBJ(const std::string& fileName);

private:

	

private: //メンバ変数

	const std::string directoryPath = "Resources";

public: //メンバ関数

	void Draw(ID3D12GraphicsCommandList* commandList, UINT rootParamIndexMaterial);

private:

	ModelData LoadObjFile(const std::string& filename);

	MaterialData LoadMaterialTemplateFile(const std::string& filename);

	

};

