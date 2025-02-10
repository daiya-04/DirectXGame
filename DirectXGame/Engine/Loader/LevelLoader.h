#pragma once
///---------------------------------------------------------------------------------------------
//
// levelLoader
// ステージ情報の読み込み
//
///---------------------------------------------------------------------------------------------

#include <string>
#include <vector>
#include <json.hpp>

#include "Vec3.h"
#include "Matrix44.h"

//レベルデータクラス
class LevelData {
public:
	//自キャラ生成データ
	struct PlayerSpawnData {
		Vector3 translation;
		Vector3 rotation;
	};

	struct EnemySpawnData {
		std::string filename;
		Vector3 translation;
		Vector3 rotation;
	};

	//オブジェクトのデータ
	struct ObjectData {
		//オブジェクト名
		std::string objectName;
		//モデルファイル名
		std::string fileName;
		//位置座標
		Vector3 translation;
		//回転
		Vector3 rotation;
		//スケール
		Vector3 scaling;
		//コライダーの中心座標
		Vector3 colliderCenter;
		//コライダーの大きさ
		Vector3 colliderSize;
		//子
		std::vector<ObjectData> children;
		//自キャラ配列
		std::vector<PlayerSpawnData> players;
		//敵キャラ配列
		std::vector<EnemySpawnData> enemies;

	};

public:
	//データのベクター配列
	std::vector<ObjectData> objectDatas_;
};

//読み込みクラス
class LevelLoader{
public:
	//ファイルの読み込み
	static LevelData* LoadFile(const std::string& filename);
private:
	//オブジェクト解析
	static void ObjectScan(nlohmann::json& object, LevelData* levelData);

};

