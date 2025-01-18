#pragma once
#include "Camera.h"
#include "GPUParticle.h"

#include <string>
#include <vector>
#include <map>

class ParticleEditor {
public:

	static ParticleEditor* GetInstance();

	void Init();

	void Update();

	void Draw(const Camera& camera);

private:
	//データファイル読み込み
	void LoadDataFile(const std::string& fileName);
	//データファイル保存
	void DataSave();
	void DebugGUI();

private:

	std::map<std::string, std::unique_ptr<GPUParticle>> particles_;

	bool isLoop_ = true;

	std::string saveFileName_;
	std::string addParticelName_;
	const std::string kDirectoryPath_ = "Resources/ParticleData/";

	std::string modelName_;
	bool isModel_ = false;

	std::vector<std::string> billboardList_;
	std::vector<std::string> emitterShapeList_;

	std::map<std::string,std::vector<std::string>> billboardComboList_;
	std::map<std::string, std::string> currentBillboardMode_;

	std::map<std::string,std::vector<std::string>> emitterShapeComboList_;
	std::map<std::string, std::string> currentEmitterShape_;

};

