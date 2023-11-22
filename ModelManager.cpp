#include "ModelManager.h"
#include "DirectXCommon.h"
#include <fstream>
#include <sstream>
#include <cassert>
#include "TextureManager.h"

using namespace Microsoft::WRL;

ModelManager* ModelManager::GetInstance() {
	static ModelManager instance;

	return &instance;
}

uint32_t ModelManager::Load(const std::string& modelName,bool isLighting) {
	return ModelManager::GetInstance()->LoadInternal(modelName,isLighting);
}

ComPtr<ID3D12Resource> ModelManager::CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes) {
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

void ModelManager::Initialize() {

	device_ = DirectXCommon::GetInstance()->GetDevice();

}

void ModelManager::SetVertexBuffers(ID3D12GraphicsCommandList* commandList, uint32_t modelHandle) {
	assert(modelHandle < kNumModel);

	commandList->IASetVertexBuffers(0, 1, &models_[modelHandle].vertexBufferView_);
}

void ModelManager::SetGraphicsRootConstantBufferView(ID3D12GraphicsCommandList* commandList, UINT rootParamIndex, uint32_t modelHandle) {
	assert(modelHandle < kNumModel);

	commandList->SetGraphicsRootConstantBufferView(rootParamIndex, models_[modelHandle].materialResource_->GetGPUVirtualAddress());
}

uint32_t ModelManager::LoadInternal(const std::string& modelName, bool isLighting) {

	assert(useModelNum_ < kNumModel);
	uint32_t handle = useModelNum_;
	models_[useModelNum_].isLighting_ = isLighting;

	auto it = std::find_if(models_.begin(), models_.end(), [&](const auto& model) {return model.name_ == modelName; });

	if (it != models_.end()) {
		handle = static_cast<uint32_t>(std::distance(models_.begin(), it));
		return handle;
	}

	models_[useModelNum_].name_ = modelName;

	LoadObjFile(modelName);

	CreateBuffer();

	useModelNum_++;
	return handle;
}

void ModelManager::LoadObjFile(const std::string& modelName) {

	std::vector<Vector4> positions;  //位置
	std::vector<Vector3> normals;  //法線
	std::vector<Vector2> texcoords;  //テクスチャ座標
	std::string line;  //ファイルから呼んだ1行を格納するもの

	filename_ = modelName + ".obj";
	directoryPath_ = "Resources/" + modelName + "/";

	std::ifstream file(directoryPath_ + filename_); //ファイルと開く
	assert(file.is_open());  //とりあえず開けなかったら止める

	while (std::getline(file, line)) {

		std::string identifier;
		std::istringstream s(line);
		s >> identifier;  //先頭の識別子を読む

		//identifierに応じた処理
		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;

			position.w = 1.0f;
			positions.push_back(position);
		}
		else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoords.push_back(texcoord);
		}
		else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;

			normals.push_back(normal);
		}
		else if (identifier == "f") {
			VertexData triangle[3];
			//面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				//頂点への要素へのindexは「位置/UV/法線」で格納されているので、分散してindexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3]{};
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');  //区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}
				//要素へのindexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];

				position.x *= -1;
				normal.x *= -1;
				texcoord.y = 1.0f - texcoord.y;

				//VertexData vertex = { position,texcoord,normal };
				//modelData.vertices.push_back(vertex);
				triangle[faceVertex] = { position,texcoord,normal };
			}
			//頂点を逆順で登録することで、周り順を逆にする
			models_[useModelNum_].vertices_.push_back(triangle[2]);
			models_[useModelNum_].vertices_.push_back(triangle[1]);
			models_[useModelNum_].vertices_.push_back(triangle[0]);
		}
		else if (identifier == "mtllib") {
			//materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			//基本的にobjファイルと同一階層にmtlは存続させるので、ディレクトリ名とファイル名を渡す
			LoadMaterialTemplateFile(materialFilename);

		}
	}

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
			models_[useModelNum_].uvHandle_ = TextureManager::GetInstance()->LoadUv(textureFilename, textureFilePath);
		}
	}

}

void ModelManager::CreateBuffer() {

	//頂点リソースを作る
	models_[useModelNum_].vertexResource_ = CreateBufferResource(device_, sizeof(VertexData) * models_[useModelNum_].vertices_.size());
	//頂点バッファビューを作成する
	models_[useModelNum_].vertexBufferView_.BufferLocation = models_[useModelNum_].vertexResource_->GetGPUVirtualAddress();  //リソースの先頭のアドレスから使う
	models_[useModelNum_].vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * models_[useModelNum_].vertices_.size()); //使用するリソースのサイズは頂点のサイズ
	models_[useModelNum_].vertexBufferView_.StrideInBytes = sizeof(VertexData);  //1頂点当たりのサイズ

	//頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	models_[useModelNum_].vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));  //書き込むためのアドレスを取得
	std::memcpy(vertexData, models_[useModelNum_].vertices_.data(), sizeof(VertexData) * models_[useModelNum_].vertices_.size());  //頂点データをリソースにコピー

	models_[useModelNum_].index_ = UINT(models_[useModelNum_].vertices_.size());

	//マテリアル用のリソースを作る。
	models_[useModelNum_].materialResource_ = CreateBufferResource(device_, sizeof(Material));
	//マテリアルにデータを書き込む
	Material* materialData = nullptr;
	//書き込むためのアドレスを取得
	models_[useModelNum_].materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	materialData->color_ = Vector4(1.0f,1.0f,1.0f,1.0f);
	materialData->enableLightnig_ = models_[useModelNum_].isLighting_;
	materialData->uvtransform_ = MakeIdentity44();

}