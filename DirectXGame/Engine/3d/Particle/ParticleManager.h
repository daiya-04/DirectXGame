#pragma once
#include "GPUParticle.h"
#include <vector>
#include <string>
#include <map>


class ParticleManager {
public:

	static ParticleManager* GetInstance();

	static GPUParticle::ParticleData Load(const std::string& fileName);

private:

	GPUParticle::ParticleData LoadInternal(const std::string& fileName);

private:

	std::map<std::string,GPUParticle::ParticleData> particleDatas_;


private:

	ParticleManager() = default;
	~ParticleManager() = default;
	ParticleManager(const ParticleManager&) = default;
	ParticleManager& operator=(const ParticleManager&) = default;

};

