#include "LevelLoader.h"

#include <fstream>
#include <cassert>
#include <numbers>

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
		objectData.translation.x = static_cast<float>(transform["translation"][0]);
		objectData.translation.y = static_cast<float>(transform["translation"][2]);
		objectData.translation.z = -static_cast<float>(transform["translation"][1]);
		//回転角
		objectData.rotation.x = (-static_cast<float>(transform["rotation"][0]) / 180.0f) * std::numbers::pi_v<float>;
		objectData.rotation.y = (-static_cast<float>(transform["rotation"][2]) / 180.0f) * std::numbers::pi_v<float>;
		objectData.rotation.z = (-static_cast<float>(transform["rotation"][1]) / 180.0f) * std::numbers::pi_v<float>;
		//スケーリング
		objectData.scaling.x = static_cast<float>(transform["scaling"][0]);
		objectData.scaling.y = static_cast<float>(transform["scaling"][2]);
		objectData.scaling.z = static_cast<float>(transform["scaling"][1]);

		if (object.contains("collider")) {
			nlohmann::json& collider = object["collider"];

			objectData.colliderCenter.x = static_cast<float>(collider["center"][0]);
			objectData.colliderCenter.y = static_cast<float>(collider["center"][2]);
			objectData.colliderCenter.z = static_cast<float>(collider["center"][1]);

			objectData.colliderSize.x = static_cast<float>(collider["size"][0]) * objectData.scaling.x;
			objectData.colliderSize.y = static_cast<float>(collider["size"][2]) * objectData.scaling.y;
			objectData.colliderSize.z = static_cast<float>(collider["size"][1]) * objectData.scaling.z;
		}

		
	}

}
