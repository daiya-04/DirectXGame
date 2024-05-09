#include "ModelManager.h"

#include "DirectXCommon.h"
#include <fstream>
#include <sstream>
#include <cassert>
#include "TextureManager.h"

using namespace Microsoft::WRL;

ID3D12Device* Model::device_ = nullptr;

void Model::CreateBuffer() {

	//頂点リソースを作る
	vertexResource_ = CreateBufferResource(device_, sizeof(VertexData) * vertices_.size());
	//頂点バッファビューを作成する
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();  //リソースの先頭のアドレスから使う
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * vertices_.size()); //使用するリソースのサイズは頂点のサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);  //1頂点当たりのサイズ

	//頂点リソースにデータを書き込む
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));  //書き込むためのアドレスを取得
	std::memcpy(vertexData, vertices_.data(), sizeof(VertexData) * vertices_.size());  //頂点データをリソースにコピー

	//indexリソースを作る
	indexResource_ = CreateBufferResource(device_, sizeof(uint32_t) * indices_.size());
	//indexバッファビューを作成する
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * indices_.size());
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	//indexリソースにデータを書き込む
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	std::memcpy(indexData_, indices_.data(), sizeof(uint32_t) * indices_.size());

	//マテリアル用のリソースを作る。
	materialResource_ = CreateBufferResource(device_, sizeof(Material));
	//マテリアルにデータを書き込む
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	materialData->color_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData->enableLightnig_ = isLighting_;
	materialData->uvtransform_ = MakeIdentity44();
	materialData->shininess_ = 10.0f;

}

ComPtr<ID3D12Resource> Model::CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes) {
	//リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapproperties{};
	uploadHeapproperties.Type = D3D12_HEAP_TYPE_UPLOAD;//UploadHeapを使う
	//リソースの設定
	D3D12_RESOURCE_DESC ResourceDesc{};
	//バッファリソース。テクスチャの場合はまた別の設定をする
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDesc.Width = sizeInBytes; //リソースのサイズ。
	//バッファの場合はこれにする決まり
	ResourceDesc.Height = 1;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.SampleDesc.Count = 1;
	//バッファの場合はこれらにする決まり
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//実際に頂点リソースを作る
	ComPtr<ID3D12Resource> Resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(&uploadHeapproperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&Resource));
	assert(SUCCEEDED(hr));

	return Resource;
}

void Model::SetVertexBuffers(ID3D12GraphicsCommandList* commandList) {
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
}

void Model::SetIndexBuffers(ID3D12GraphicsCommandList* commandList) {
	commandList->IASetIndexBuffer(&indexBufferView_);
}

void Model::SetGraphicsRootConstantBufferView(ID3D12GraphicsCommandList* commandList, UINT rootParamIndex) {
	commandList->SetGraphicsRootConstantBufferView(rootParamIndex, materialResource_->GetGPUVirtualAddress());
}

void Model::SetDevice() {
	device_ = DirectXCommon::GetInstance()->GetDevice();
}

ModelManager* ModelManager::GetInstance() {
	static ModelManager instance;

	return &instance;
}

std::shared_ptr<Model> ModelManager::LoadOBJ(const std::string& modelName, bool isLighting) {
	return ModelManager::GetInstance()->LoadInternal(modelName, isLighting, "obj");
}

std::shared_ptr<Model> ModelManager::LoadGLTF(const std::string& modelName, bool isLighting) {
	return ModelManager::GetInstance()->LoadInternal(modelName, isLighting, "gltf");
}

std::shared_ptr<Model> ModelManager::LoadInternal(const std::string& modelName, bool isLighting, const std::string& extension) {
  
	assert(useModelNum_ < kNumModel);
	uint32_t handle = useModelNum_;
	

	auto it = std::find_if(models_.begin(), models_.end(), [&](const auto& model) {return model->name_ == modelName; });

	if (it != models_.end()) {
		handle = static_cast<uint32_t>(std::distance(models_.begin(), it));
		return models_[handle];
	}

	models_.push_back(std::unique_ptr<Model>(new Model()));

	models_[handle]->name_ = modelName;
	models_[useModelNum_]->isLighting_ = isLighting;

	if (extension == "obj") {
		LoadObjFile(modelName);
	}else if (extension == "gltf") {
		LoadGltfFile(modelName);
	}
	

	models_[handle]->CreateBuffer();

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

	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());
		assert(mesh->HasTextureCoords(0));
		models_.back()->vertices_.resize(mesh->mNumVertices);
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];

			models_.back()->vertices_[vertexIndex].pos_ = { -position.x,position.y,position.z,1.0f };
			models_.back()->vertices_[vertexIndex].normal = { -normal.x,normal.y,normal.z };
			models_.back()->vertices_[vertexIndex].uv_ = { texcoord.x,texcoord.y };
		}
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);
			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				models_.back()->indices_.push_back(vertexIndex);
			}
		}
	}

	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			std::string materialFilename;
			materialFilename = directoryPath_ + textureFilePath.C_Str();
			models_.back()->uvHandle_ = TextureManager::GetInstance()->LoadUv(textureFilePath.C_Str(), materialFilename);
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

	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());
		assert(mesh->HasTextureCoords(0));
		models_.back()->vertices_.resize(mesh->mNumVertices);
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];

			models_.back()->vertices_[vertexIndex].pos_ = { -position.x,position.y,position.z,1.0f };
			models_.back()->vertices_[vertexIndex].normal = { -normal.x,normal.y,normal.z };
			models_.back()->vertices_[vertexIndex].uv_ = { texcoord.x,texcoord.y };
		}
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);
			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				models_.back()->indices_.push_back(vertexIndex);
			}
		}
	}

	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			std::string materialFilename;
			materialFilename = directoryPath_ + textureFilePath.C_Str();
			models_.back()->uvHandle_ = TextureManager::GetInstance()->LoadUv(textureFilePath.C_Str(), materialFilename);
		}
	}

	models_.back()->rootNode_ = ReadNode(scene->mRootNode);

}

void ModelManager::LoadMaterialTemplateFile(const std::string& fileName) {

	std::string textureFilePath;
	std::string line; //ファイルから読んだ1行を格納するもの
	std::ifstream file(directoryPath_ + fileName); //ファイルを開く
	assert(file.is_open()); //開かなかったら止める

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		//identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			//連結してファイルパスにする
			textureFilePath = directoryPath_ + textureFilename;
			models_[useModelNum_]->uvHandle_ = TextureManager::GetInstance()->LoadUv(textureFilename, textureFilePath);
		}
	}

}

Model::Node ModelManager::ReadNode(aiNode* node) {

	Model::Node result;
	aiVector3D translate, scale;
	aiQuaternion rotate;
	node->mTransformation.Decompose(scale, rotate, translate); //assimpの行列からSRTを抽出する
	result.transform_.translate_ = { -translate.x,translate.y ,translate.z }; //x軸を反転、さらに回転方向が逆なので軸を反転させる
	result.transform_.rotate_ = { rotate.x, -rotate.y, -rotate.z, rotate.w }; //x軸を反転
	result.transform_.scale_ = { scale.x,scale.y,scale.z }; //scaleはそのまま
	result.localMatrix_ = MakeTranslateMatrix(result.transform_.translate_) * result.transform_.rotate_.MakeRotateMatrix() * MakeScaleMatrix(result.transform_.scale_);

	result.name_ = node->mName.C_Str(); //Node名を格納
	result.children_.resize(node->mNumChildren); //子供の数だけ確保
	for (size_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
		result.children_[childIndex] = ReadNode(node->mChildren[childIndex]);
	}

	return result;
}

Skeleton Skeleton::Create(const Model::Node& rootNade) {
	Skeleton skeleton;
	skeleton.root_;

	for (const Joint& joint : skeleton.joints_) {
		skeleton.jointMap.emplace(joint.name_, joint.index_);
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
		joint.localMat_ = MakeTranslateMatrix(joint.transform_.translate_) * joint.transform_.rotate_.MakeRotateMatrix() * MakeScaleMatrix(joint.transform_.scale_);
		if (joint.parent_) {
			joint.skeletonSpaceMat_ = joint.localMat_ * joints_[*joint.parent_].skeletonSpaceMat_;
		}
		else {
			joint.skeletonSpaceMat_ = joint.localMat_;
		}


	}

}
