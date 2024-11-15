#include "ParticleManager.h"

#include <json.hpp>
#include <fstream>
#include <cassert>
#include <Windows.h>

using namespace nlohmann;

ParticleManager* ParticleManager::GetInstance() {
	static ParticleManager instance;

	return &instance;
}

GPUParticle::ParticleData ParticleManager::Load(const std::string& fileName) {
	return ParticleManager::GetInstance()->LoadInternal(fileName);
}

GPUParticle::ParticleData ParticleManager::LoadInternal(const std::string& fileName) {

	//既に読み込んでいたらリターン
	/*if (particleDatas_.find(fileName) != particleDatas_.end()) {
		return particleDatas_[fileName];
	}*/


	//ディレクトリパス
	const std::string kDirectoryPath = "Resources/ParticleData/";
	//ファイルの拡張子
	const std::string kExtension = ".json";

	GPUParticle::ParticleData data{};

	//フルパスの取得
	const std::string fullPath = kDirectoryPath + fileName + kExtension;

	std::ifstream file;

	file.open(fullPath);

	//open失敗
	if (file.fail()) {
		std::string message = "Failed open data file.";
		MessageBoxA(nullptr, message.c_str(), "ParticleManager", 0);
		return data;
	}

	json root;
	//json文字列からjsonのデータ構造に展開
	file >> root;
	//ファイルを閉じる
	file.close();

	//データがあるかのチェック
	if (!root.contains(fileName)) {
		std::string message = "The file contains no data.";
		MessageBoxA(nullptr, message.c_str(), "ParticleEditor", 0);
		return data;
	}

	//データ設定
	data.isLoop_ = root[fileName]["isLoop"].get<bool>();
	data.textureName_ = root[fileName]["textureName"].get<std::string>();

	auto& emitterData = data.emitter_;
	json& emitterRoot = root[fileName]["Emitter"];
	
	json emitTranslate = emitterRoot["Translate"];
	emitterData.translate = Vector3(static_cast<float>(emitTranslate[0]), static_cast<float>(emitTranslate[1]), static_cast<float>(emitTranslate[2]));
	json emitSize = emitterRoot["Size"];
	emitterData.size = Vector3(static_cast<float>(emitSize[0]), static_cast<float>(emitSize[1]), static_cast<float>(emitSize[2]));
	emitterData.radius = emitterRoot["Radius"].get<float>();
	emitterData.scale = emitterRoot["Scale"].get<float>();
	emitterData.rotate = emitterRoot["Rotate"].get<float>();
	emitterData.count = emitterRoot["Count"].get<uint32_t>();
	emitterData.frequency = emitterRoot["Frequency"].get<float>();
	json emitColor = emitterRoot["Color"];
	emitterData.color = Vector4(static_cast<float>(emitColor[0]), static_cast<float>(emitColor[1]), static_cast<float>(emitColor[2]), static_cast<float>(emitColor[3]));
	emitterData.lifeTime = emitterRoot["LifeTime"].get<float>();
	emitterData.speed = emitterRoot["Speed"].get<float>();
	emitterData.emitterType = emitterRoot["EmitterType"].get<uint32_t>();
	if (emitterRoot.contains("BillboardType")) {
		emitterData.billboardType = emitterRoot["BillboardType"].get<uint32_t>();
	}

	auto& overLifeTimeData = data.overLifeTime_;
	json& overLifeTimeRoot = root[fileName]["OverLifeTime"];

	overLifeTimeData.isConstantVelocity = overLifeTimeRoot["isConstantVelocity"].get<uint32_t>();
	json velocity = overLifeTimeRoot["Velocity"];
	overLifeTimeData.velocity = Vector3(static_cast<float>(velocity[0]), static_cast<float>(velocity[1]), static_cast<float>(velocity[2]));
	overLifeTimeData.isTransVelocity = overLifeTimeRoot["isTransVelocity"].get<uint32_t>();
	json startVelo = overLifeTimeRoot["startVelocity"];
	overLifeTimeData.startVelocity = Vector3(static_cast<float>(startVelo[0]), static_cast<float>(startVelo[1]), static_cast<float>(startVelo[2]));
	json endVelo = overLifeTimeRoot["endVelocity"];
	overLifeTimeData.endVelocity = Vector3(static_cast<float>(endVelo[0]), static_cast<float>(endVelo[1]), static_cast<float>(endVelo[2]));
	overLifeTimeData.isScale = overLifeTimeRoot["isScale"].get<uint32_t>();
	overLifeTimeData.startScale = overLifeTimeRoot["startScale"].get<float>();
	overLifeTimeData.endScale = overLifeTimeRoot["endScale"].get<float>();
	overLifeTimeData.isColor = overLifeTimeRoot["isColor"].get<uint32_t>();
	json startColor = overLifeTimeRoot["startColor"];
	overLifeTimeData.startColor = Vector3(static_cast<float>(startColor[0]), static_cast<float>(startColor[1]), static_cast<float>(startColor[2]));
	json endColor = overLifeTimeRoot["endColor"];
	overLifeTimeData.endColor = Vector3(static_cast<float>(endColor[0]), static_cast<float>(endColor[1]), static_cast<float>(endColor[2]));
	overLifeTimeData.isAlpha = overLifeTimeRoot["isAlpha"].get<uint32_t>();
	overLifeTimeData.startAlpha = overLifeTimeRoot["startAlpha"].get<float>();
	overLifeTimeData.midAlpha = overLifeTimeRoot["midAlpha"].get<float>();
	overLifeTimeData.endAlpha = overLifeTimeRoot["endAlpha"].get<float>();
	overLifeTimeData.isTransSpeed = overLifeTimeRoot["isTransSpeed"].get<uint32_t>();
	overLifeTimeData.startSpeed = overLifeTimeRoot["startSpeed"].get<float>();
	overLifeTimeData.endSpeed = overLifeTimeRoot["endSpeed"].get<float>();
	overLifeTimeData.gravity = overLifeTimeRoot["Gravity"].get<float>();

	particleDatas_[fileName] = data;

	return particleDatas_[fileName];
}

