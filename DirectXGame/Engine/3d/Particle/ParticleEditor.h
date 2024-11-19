#pragma once
#include "Camera.h"
#include "GPUParticle.h"

#include <string>

class ParticleEditor {
public:

	static ParticleEditor* GetInstance();

	void Init();

	void Update();

	void Draw(const Camera& camera);

private:

	void LoadDataFile(const std::string& fileName);

	void DataSave();

	void DebugGUI();

private:

	std::unique_ptr<GPUParticle> particle_;

	std::string saveFileName_;
	const std::string kDirectoryPath_ = "Resources/ParticleData/";


};

