#include "Model.h"
#include <fstream>
#include <sstream>
#include <cassert>

using namespace Microsoft::WRL;

ComPtr<ID3D12Resource> Model::vertexResource_;

Model* Model::LoadOBJ(const std::string& fileName) {

	Model* model = new Model();

	ModelData modelData = model->LoadObjFile(fileName);

	
	return model;
}

void Model::Draw(ID3D12GraphicsCommandList* commandList, UINT rootParamIndexMaterial) {



}

Model::ModelData Model::LoadObjFile(const std::string& filename) {

	ModelData modelData; //構築するModelData
	std::vector<Vector4> positions;  //位置
	std::vector<Vector3> normals;  //法線
	std::vector<Vector2> texcoords;  //テクスチャ座標
	std::string line;  //ファイルから呼んだ1行を格納するもの

	std::ifstream file(directoryPath + "/" + filename); //ファイルと開く
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
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		}
		else if (identifier == "mtllib") {
			//materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			//基本的にobjファイルと同一階層にmtlは存続させるので、ディレクトリ名とファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(materialFilename);
		}
	}

	return modelData;
}

Model::MaterialData Model::LoadMaterialTemplateFile(const std::string& filename) {

	MaterialData materialData; //構築するMatrialData
	std::string line; //ファイルから読んだ1行を格納するもの
	std::ifstream file(directoryPath + "/" + filename); //ファイルを開く
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
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}
	return materialData;
}

