#include "ModelManager.h"

#include "DirectXCommon.h"
#include <fstream>
#include <sstream>
#include <cassert>
#include "TextureManager.h"
#include "ModelManager.h"
#include "Line.h"
#include "ShapesDraw.h"

using namespace Microsoft::WRL;

ModelManager* ModelManager::GetInstance() {
	static ModelManager instance;

	return &instance;
}

std::shared_ptr<Model> ModelManager::LoadOBJ(const std::string& modelName) {
	return ModelManager::GetInstance()->LoadInternal(modelName, "obj");
}

std::shared_ptr<Model> ModelManager::LoadGLTF(const std::string& modelName) {
	return ModelManager::GetInstance()->LoadInternal(modelName, "gltf");
}

std::shared_ptr<Model> ModelManager::LoadInternal(const std::string& modelName, const std::string& extension) {
  
	assert(useModelNum_ < kNumModel);
	uint32_t handle = useModelNum_;
	

	auto it = std::find_if(models_.begin(), models_.end(), [&](const auto& model) {return model->name_ == modelName; });

	if (it != models_.end()) {
		handle = static_cast<uint32_t>(std::distance(models_.begin(), it));
		return models_[handle];
	}

	if (extension == "obj") {
		LoadObjFile(modelName);
	}else if (extension == "gltf") {
		LoadGltfFile(modelName);
	}
	
	models_[handle]->name_ = modelName;
	for(auto& mesh : models_[handle]->meshes_){
		mesh.Init();
		if (extension == "gltf") {
			mesh.SkinnedInit();
		}
	}

	useModelNum_++;
	return models_[handle];
}

void ModelManager::LoadObjFile(const std::string& modelName) {

	std::vector<Vector4> positions;  //位置
	std::vector<Vector3> normals;  //法線
	std::vector<Vector2> texcoords;  //テクスチャ座標
	std::string line;  //ファイルから呼んだ1行を格納するもの

	Assimp::Importer importer;
	filename_ = modelName + ".obj";
	directoryPath_ = "Resources/model/" + modelName + "/";
	std::string filePath = directoryPath_ + filename_;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes());

	auto& model = models_.emplace_back(std::make_shared<Model>());

	model->meshes_.resize(scene->mNumMeshes);
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());
		assert(mesh->HasTextureCoords(0));
		model->meshes_[meshIndex].vertices_.resize(mesh->mNumVertices);
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];

			model->meshes_[meshIndex].vertices_[vertexIndex].pos_ = { -position.x,position.y,position.z,1.0f };
			model->meshes_[meshIndex].vertices_[vertexIndex].normal = { -normal.x,normal.y,normal.z };
			model->meshes_[meshIndex].vertices_[vertexIndex].uv_ = { texcoord.x,texcoord.y };
		}
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);
			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				model->meshes_[meshIndex].indices_.push_back(vertexIndex);
			}
		}
		
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			std::string materialFilename;
			materialFilename = directoryPath_ + textureFilePath.C_Str();
			Material material;
			material.SetUVHandle(TextureManager::GetInstance()->LoadUv(textureFilePath.C_Str(), materialFilename));
			model->meshes_[meshIndex].SetMaterial(material);
		}
	}

}

void ModelManager::LoadGltfFile(const std::string& modelName) {

	std::vector<Vector4> positions;  //位置
	std::vector<Vector3> normals;  //法線
	std::vector<Vector2> texcoords;  //テクスチャ座標
	std::string line;  //ファイルから呼んだ1行を格納するもの

	Assimp::Importer importer;
	filename_ = modelName + ".gltf";
	directoryPath_ = "Resources/model/" + modelName + "/";
	std::string filePath = directoryPath_ + filename_;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes());

	auto& model = models_.emplace_back(std::make_shared<Model>());

	model->meshes_.resize(scene->mNumMeshes);
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());
		assert(mesh->HasTextureCoords(0));
		model->meshes_[meshIndex].vertices_.resize(mesh->mNumVertices);
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];

			model->meshes_[meshIndex].vertices_[vertexIndex].pos_ = { -position.x,position.y,position.z,1.0f };
			model->meshes_[meshIndex].vertices_[vertexIndex].normal = { -normal.x,normal.y,normal.z };
			model->meshes_[meshIndex].vertices_[vertexIndex].uv_ = { texcoord.x,texcoord.y };
		}
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);
			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				model->meshes_[meshIndex].indices_.push_back(vertexIndex);
			}
		}
		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
			aiBone* bone = mesh->mBones[boneIndex];
			std::string jointName = bone->mName.C_Str();
			Model::JointWeightData& jointWeightData = model->skinClusterData_[jointName];

			aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();
			aiVector3D translate, scale;
			aiQuaternion rotate;
			bindPoseMatrixAssimp.Decompose(scale, rotate, translate);
			Quaternion q = { rotate.x, -rotate.y, -rotate.z, rotate.w };
			Matrix4x4 TMat = MakeTranslateMatrix({ -translate.x, translate.y, translate.z });
			Matrix4x4 SMat = MakeScaleMatrix({ scale.x,scale.y,scale.z });
			Matrix4x4 bindPoseMat = SMat * q.MakeRotateMatrix() * TMat;
			jointWeightData.inverseBindPoseMatrix_ = bindPoseMat.Inverse();
			for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
				jointWeightData.vertexWeights_.push_back({ bone->mWeights[weightIndex].mWeight,bone->mWeights[weightIndex].mVertexId });
			}
		}

		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			std::string materialFilename;
			materialFilename = directoryPath_ + textureFilePath.C_Str();
			Material material;
			material.SetUVHandle(TextureManager::GetInstance()->LoadUv(textureFilePath.C_Str(), materialFilename));
			model->meshes_[meshIndex].SetMaterial(material);
		}
	}

	model->rootNode_ = ReadNode(scene->mRootNode);

}

//void ModelManager::LoadMaterialTemplateFile(const std::string& fileName) {
//
//	std::string textureFilePath;
//	std::string line; //ファイルから読んだ1行を格納するもの
//	std::ifstream file(directoryPath_ + fileName); //ファイルを開く
//	assert(file.is_open()); //開かなかったら止める
//
//	while (std::getline(file, line)) {
//		std::string identifier;
//		std::istringstream s(line);
//		s >> identifier;
//
//		//identifierに応じた処理
//		if (identifier == "map_Kd") {
//			std::string textureFilename;
//			s >> textureFilename;
//			//連結してファイルパスにする
//			textureFilePath = directoryPath_ + textureFilename;
//			models_[useModelNum_]->uvHandle_ = TextureManager::GetInstance()->LoadUv(textureFilename, textureFilePath);
//		}
//	}
//
//}

Model::Node ModelManager::ReadNode(aiNode* node) {

	Model::Node result;
	aiVector3D translate, scale;
	aiQuaternion rotate;
	node->mTransformation.Decompose(scale, rotate, translate); //assimpの行列からSRTを抽出する
	result.transform_.translate_ = { -translate.x,translate.y ,translate.z }; //x軸を反転、さらに回転方向が逆なので軸を反転させる
	result.transform_.rotate_ = { rotate.x, -rotate.y, -rotate.z, rotate.w }; //x軸を反転
	result.transform_.scale_ = { scale.x,scale.y,scale.z }; //scaleはそのまま
	result.localMatrix_ = MakeScaleMatrix(result.transform_.scale_) * result.transform_.rotate_.MakeRotateMatrix() * MakeTranslateMatrix(result.transform_.translate_);

	result.name_ = node->mName.C_Str(); //Node名を格納
	result.children_.resize(node->mNumChildren); //子供の数だけ確保
	for (size_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
		result.children_[childIndex] = ReadNode(node->mChildren[childIndex]);
	}

	return result;
}

Skeleton Skeleton::Create(const Model::Node& rootNade) {
	Skeleton skeleton;
	skeleton.root_ = CreateJoint(rootNade, {}, skeleton.joints_);

	for (const Joint& joint : skeleton.joints_) {
		skeleton.jointMap_.emplace(joint.name_, joint.index_);
	}

	return skeleton;
}

int32_t Skeleton::CreateJoint(const Model::Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints) {

	Joint joint;
	joint.name_ = node.name_;
	joint.localMat_ = node.localMatrix_;
	joint.skeletonSpaceMat_ = MakeIdentity44();
	joint.transform_ = node.transform_;
	joint.index_ = int32_t(joints.size());
	joint.parent_ = parent;
	joints.push_back(joint);
	for (const Model::Node child : node.children_) {
		int32_t childIndex = CreateJoint(child, joint.index_, joints);
		joints[joint.index_].children_.push_back(childIndex);
	}

	return joint.index_;
}

void Skeleton::Update() {

	for (Joint& joint : joints_) {
		joint.localMat_ = MakeScaleMatrix(joint.transform_.scale_) * joint.transform_.rotate_.MakeRotateMatrix() * MakeTranslateMatrix(joint.transform_.translate_);
		if (joint.parent_) {
			joint.skeletonSpaceMat_ = joint.localMat_ * joints_[*joint.parent_].skeletonSpaceMat_;
		}
		else {
			joint.skeletonSpaceMat_ = joint.localMat_;
		}
	}
}

void Skeleton::Draw(const WorldTransform& worldTransform, const Camera& camera) {
#ifdef _DEBUG
	for (auto& joint : joints_) {

		if (!joint.parent_) { continue; }

		Matrix4x4 startPointMat = joint.skeletonSpaceMat_ * worldTransform.matWorld_;
		Matrix4x4 endPointMat = joints_[*joint.parent_].skeletonSpaceMat_ * worldTransform.matWorld_;

		Vector3 start = {
			startPointMat.m[3][0],
			startPointMat.m[3][1],
			startPointMat.m[3][2],
		};

		Vector3 end = {
			endPointMat.m[3][0],
			endPointMat.m[3][1],
			endPointMat.m[3][2],
		};

		Line::Draw(start, end, camera);

}
#endif // _DEBUG
}

Vector3 Skeleton::GetSkeletonPos(const std::string& jointName) {

	int32_t jointIndex = jointMap_[jointName];

	Vector3 jointPos = {
		joints_[jointIndex].skeletonSpaceMat_.m[3][0],
		joints_[jointIndex].skeletonSpaceMat_.m[3][1],
		joints_[jointIndex].skeletonSpaceMat_.m[3][2],
	};

	return jointPos;
}
