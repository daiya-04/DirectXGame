#pragma once
#include <string>
#include <vector>
#include <json.hpp>

#include "Vec3.h"
#include "Matrix44.h"

class LevelData {
public:

	struct ObjectData {

		std::string objectName;

		std::string fileName;

		Vector3 translation;

		Vector3 rotation;

		Vector3 scaling;

		Vector3 colliderSize;

		std::vector<ObjectData> children;

	};

public:
	std::vector<ObjectData> objectDatas_;
};

class LevelLoader{
public:

	static LevelData* LoadFile(const std::string& filename);
private:

	static void ObjectScan(nlohmann::json& object, LevelData* levelData);

};

