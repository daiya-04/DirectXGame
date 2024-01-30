#pragma once
#include <string>
#include <vector>

#include "Vec3.h"
#include "Matrix44.h"

class LevelData {
public:

	struct ObjectData {

		std::string fileName;

		Vector3 translation;

		Vector3 rotation;

		Vector3 scaling;

	};

public:
	std::vector<ObjectData> objects;
};

class LevelLoader{
public:

	static LevelData* LoadFile(const std::string& filename);

};

