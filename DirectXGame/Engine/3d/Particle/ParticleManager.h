#pragma once
#include "GPUParticle.h"
#include <vector>
#include <string>
#include <map>


class ParticleManager {
public:

	static ParticleManager* GetInstance();
	//データファイルの読み込み
	static std::map<std::string, std::unique_ptr<DaiEngine::GPUParticle>> Load(const std::string& fileName);

private:
	//データファイルの読み込み
	std::map<std::string, std::unique_ptr<DaiEngine::GPUParticle>> LoadInternal(const std::string& fileName);

private:

	struct Data {
		std::map<std::string, DaiEngine::GPUParticle::ParticleData> particle;
	};

	std::map<std::string,Data> datas_;


private:

	ParticleManager() = default;
	~ParticleManager() = default;
	ParticleManager(const ParticleManager&) = default;
	ParticleManager& operator=(const ParticleManager&) = default;

};

