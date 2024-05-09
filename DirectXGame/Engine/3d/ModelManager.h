#pragma once
#include <d3d12.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <wrl.h>
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <optional>
#include <map>
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Quaternion.h"
#include "Matrix44.h"

struct QuaternionTransform {
	Vector3 translate_;
	Quaternion rotate_;
	Vector3 scale_;
};

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

	struct Node {
		QuaternionTransform transform_;
		Matrix4x4 localMatrix_;
		std::string name_;
		std::vector<Node> children_;
	};

private:

	//リソースの生成
	static ComPtr<ID3D12Resource> CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes);

public:

	static void SetDevice();

	void CreateBuffer();

	void SetVertexBuffers(ID3D12GraphicsCommandList* commandList);
	void SetIndexBuffers(ID3D12GraphicsCommandList* commandList);

	void SetGraphicsRootConstantBufferView(ID3D12GraphicsCommandList* commandList, UINT rootParamIndex);

	void SetColor(const Vector4& color) { materialData->color_ = color; }

	uint32_t GetUvHandle() const { return uvHandle_; }

public:

	std::vector<VertexData> vertices_;
	std::vector<uint32_t> indices_;
	Node rootNode_;
	//uv
	int32_t uvHandle_ = 0;
	//Lightingの有無
	bool isLighting_ = true;
	//modelファイルの名前
	std::string name_;

private:

	static ID3D12Device* device_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	ComPtr<ID3D12Resource> vertexResource_;
	VertexData* vertexData = nullptr;

	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	ComPtr<ID3D12Resource> indexResource_;
	uint32_t* indexData_ = nullptr;
	
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
	std::vector<std::shared_ptr<Model>> models_;
	uint32_t useModelNum_= 0;

public:

	static ModelManager* GetInstance();

	static std::shared_ptr<Model> LoadOBJ(const std::string& modelName, bool isLighting = true);

	static std::shared_ptr<Model> LoadGLTF(const std::string& modelName, bool isLighting = true);

private:

	std::shared_ptr<Model> LoadInternal(const std::string& modelName, bool isLighting, const std::string& extension);

	void LoadObjFile(const std::string& modelName);

	void LoadGltfFile(const std::string& modelName);

	void LoadMaterialTemplateFile(const std::string& fileName);

	Model::Node ReadNode(aiNode* node);

private:

	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(const ModelManager&) = delete;
	ModelManager& operator=(const ModelManager&) = delete;

};

class Skeleton {
public:

	struct Joint {
		QuaternionTransform transform_;
		Matrix4x4 localMat_;
		Matrix4x4 skeletonSpaceMat_;
		std::string name_;
		std::vector<int32_t> children_; //子JointのIndexのリスト。いなければ空
		int32_t index_; //自身のindex
		std::optional<int32_t> parent_; //親Jointのindex。いなければnull
	};

public:

	static Skeleton Create(const Model::Node& rootNode);

	void Update();

private:

	int32_t CreateJoint(const Model::Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);

public:

	int32_t root_; //RootJointのIndex
	std::map<std::string, int32_t> jointMap; //Joint名とIndexとの辞書
	std::vector<Joint> joints_;

};
