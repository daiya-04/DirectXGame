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
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"
#include "WorldTransform.h"

struct QuaternionTransform {
	Vector3 translate_;
	Quaternion rotate_;
	Vector3 scale_;
};

namespace DaiEngine {
	class Model {
	private:
		template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

		friend class ModelManager;

	public:

		struct Node {
			QuaternionTransform transform_;
			Matrix4x4 localMatrix_;
			std::string name_;
			std::vector<Node> children_;
		};

		struct VertexWeightData {
			float weight_;
			uint32_t vertexIndex_;
		};

		struct JointWeightData {
			Matrix4x4 inverseBindPoseMatrix_;
			std::vector<VertexWeightData> vertexWeights_;
		};

	public:


	public:


		Node rootNode_;

		std::vector<Mesh> meshes_;


		//modelファイルの名前
		std::string name_;

		std::map<std::string, JointWeightData> skinClusterData_;

	};

	class ModelManager {
	private:
		template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	public:

		static const size_t kNumModel = 128;

	private:


		std::string filename_;
		std::string directoryPath_;
		std::vector<std::shared_ptr<Model>> models_;
		uint32_t useModelNum_ = 0;

	public:

		static ModelManager* GetInstance();
		//.objファイルの読み込み
		static std::shared_ptr<Model> LoadOBJ(const std::string& modelName);
		//.gltfファイルの読み込み
		static std::shared_ptr<Model> LoadGLTF(const std::string& modelName);

	private:

		std::shared_ptr<Model> LoadInternal(const std::string& modelName, const std::string& extension);

		void LoadObjFile(const std::string& modelName);

		void LoadGltfFile(const std::string& modelName);

		//void LoadMaterialTemplateFile(const std::string& fileName);
		//ノードの読み込み
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

		void Draw(const WorldTransform& worldTransform, const Camera& camera);

		Vector3 GetSkeletonPos(const std::string& jointName);

	private:
		//ジョイント生成
		static int32_t CreateJoint(const Model::Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);

	public:

		int32_t root_; //RootJointのIndex
		std::map<std::string, int32_t> jointMap_; //Joint名とIndexとの辞書
		std::vector<Joint> joints_;

	};

}

