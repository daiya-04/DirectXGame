#include "LevelLoader.h"

#include <fstream>
#include <cassert>

LevelData* LevelLoader::LoadFile(const std::string& filename) {

	//ディレクトリパス
	const std::string kDirectoryPath = "Resources/levels/";
	//ファイル拡張子
	const std::string kExtension = ".json";

	//連結フルパスを得る
	const std::string fullPath = kDirectoryPath + filename + kExtension;

	//ファイルストリーム
	std::ifstream file;

	//ファイルを開く
	file.open(fullPath);
	//ファイルオープン失敗をチェック
	if (file.fail()) {
		assert(0);
	}

	//JSON文字列から解凍したデータ
	nlohmann::json deserialized;

	//解凍
	file >> deserialized;

	//正しいレベルデータファイルかチェック
	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	//"name"を文字列として取得
	std::string name = deserialized["name"].get<std::string>();
	//正しいレベルデータファイルかチェック
	assert(name.compare("scene") == 0);

	//レベルデータ格納用インスタンスを生成
	LevelData* levelData = new LevelData();

	//"objects"の全オブジェクトを走査
	for (nlohmann::json& object : deserialized["objects"]) {

		

		ObjectScan(object, levelData);

		//オブジェクト走査を再帰関数にまとめ、再帰呼出で枝を走査する
		if (object.contains("children")) {
			for (auto& child : object["children"]) {
				ObjectScan(child, levelData);
			}
		}

	}

	return levelData;
}

void LevelLoader::ObjectScan(nlohmann::json& object, LevelData* levelData) {

	assert(object.contains("type"));
	//種別を取得
	std::string type = object["type"].get<std::string>();

	//MESH
	if (type.compare("MESH") == 0 || type.compare("ARMATURE") == 0) {

		//要素追加
		levelData->objectDatas_.emplace_back(LevelData::ObjectData{});
		LevelData::ObjectData& objectData = levelData->objectDatas_.back();

		if (object.contains("file_name")) {
			//ファイル名
			objectData.fileName = object["file_name"];
		}
		if (object.contains("object_name")) {
			objectData.objectName = object["object_name"];
		}

		//トランスフォームのパラメータ読み込み
		nlohmann::json& transform = object["transform"];
		//平行移動
		objectData.translation.x = (float)transform["translation"][1];
		objectData.translation.y = (float)transform["translation"][2];
		objectData.translation.z = -(float)transform["translation"][0];
		//回転角
		objectData.rotation.x = -(float)transform["rotation"][1];
		objectData.rotation.y = -(float)transform["rotation"][2];
		objectData.rotation.z = (float)transform["rotation"][0];
		//スケーリング
		objectData.scaling.x = (float)transform["scaling"][1];
		objectData.scaling.y = (float)transform["scaling"][2];
		objectData.scaling.z = (float)transform["scaling"][0];

		//
		if (object.contains("status")) {
			nlohmann::json& status = object["status"];
			objectData.status.HP = (int32_t)status["HP"];
			objectData.status.power = (int32_t)status["power"];
			objectData.status.defense = (int32_t)status["difense"];
		}
	}

}
