#include "ParticleEditor.h"

#include "ImGuiManager.h"
#include "TextureManager.h"
#include "ParticleManager.h"
#include <json.hpp>
#include <fstream>
#include <cassert>
#include <Windows.h>

using namespace nlohmann;

ParticleEditor* ParticleEditor::GetInstance() {
	static ParticleEditor instance;

	return &instance;
}

void ParticleEditor::Init() {

	//particle_.reset(GPUParticle::Create(TextureManager::Load("circle.png"), 100000));
	saveFileName_ = "testParticle";
	//LoadDataFile(saveFileName_);
	

}

void ParticleEditor::Update() {
	DebugGUI();

	for (auto& [group, particle] : particles_) {
		particle->Update();
	}
}

void ParticleEditor::Draw(const Camera& camera) {
	for (auto& [group, particle] : particles_) {
		particle->Draw(camera);
	}
}

void ParticleEditor::DataSave() {
	//ファイル名が入力されていなかったら
	if (saveFileName_.empty()) {
		std::string message = "FileName is empty.";
		MessageBoxA(nullptr, message.c_str(), "ParticleEditor", 0);
		return;
	}

	json root;

	root = json::object();
	//jsonオブジェクト登録
	root[saveFileName_] = json::object();

	for (auto& [group, particle] : particles_) {
		json& particleRoot = root[saveFileName_][group];
		const auto& data = particle->particleData_;

		//rootに保存するパラメータの登録
		particleRoot["isLoop"] = data.isLoop_;
		particleRoot["textureName"] = data.textureName_;

		json& emitterRoot = particleRoot["Emitter"];
		const auto& emitter = data.emitter_;

		Vector3 emitTranslate = emitter.translate;
		emitterRoot["Translate"] = json::array({ emitTranslate.x, emitTranslate.y, emitTranslate.z });
		Vector3 emitSize = emitter.size;
		emitterRoot["Size"] = json::array({ emitSize.x, emitSize.y, emitSize.z });
		emitterRoot["Radius"] = emitter.radius;
		emitterRoot["Scale"] = emitter.scale;
		emitterRoot["Rotate"] = emitter.rotate;
		emitterRoot["Count"] = emitter.count;
		emitterRoot["Frequency"] = emitter.frequency;
		Vector4 emitColor = emitter.color;
		emitterRoot["Color"] = json::array({ emitColor.x, emitColor.y, emitColor.z, emitColor.w });
		emitterRoot["LifeTime"] = emitter.lifeTime;
		emitterRoot["Speed"] = emitter.speed;
		emitterRoot["EmitterType"] = emitter.emitterType;
		emitterRoot["BillboardType"] = emitter.billboardType;

		json& overLifeTimeRoot = particleRoot["OverLifeTime"];
		const auto& overLifeTime = data.overLifeTime_;

		overLifeTimeRoot["isConstantVelocity"] = overLifeTime.isConstantVelocity;
		Vector3 velocity = overLifeTime.velocity;
		overLifeTimeRoot["Velocity"] = json::array({ velocity.x, velocity.y, velocity.z });
		overLifeTimeRoot["isTransVelocity"] = overLifeTime.isTransVelocity;
		Vector3 startVelocity = overLifeTime.startVelocity;
		overLifeTimeRoot["startVelocity"] = json::array({ startVelocity.x, startVelocity.y, startVelocity.z });
		Vector3 endVelocity = overLifeTime.endVelocity;
		overLifeTimeRoot["endVelocity"] = json::array({ endVelocity.x, endVelocity.y, endVelocity.z });
		overLifeTimeRoot["isScale"] = overLifeTime.isScale;
		overLifeTimeRoot["startScale"] = overLifeTime.startScale;
		overLifeTimeRoot["endScale"] = overLifeTime.endScale;
		overLifeTimeRoot["isColor"] = overLifeTime.isColor;
		Vector3 startColor = overLifeTime.startColor;
		overLifeTimeRoot["startColor"] = json::array({ startColor.x, startColor.y, startColor.z });
		Vector3 endColor = overLifeTime.endColor;
		overLifeTimeRoot["endColor"] = json::array({ endColor.x, endColor.y, endColor.z });
		overLifeTimeRoot["isAlpha"] = overLifeTime.isAlpha;
		overLifeTimeRoot["startAlpha"] = overLifeTime.startAlpha;
		overLifeTimeRoot["midAlpha"] = overLifeTime.midAlpha;
		overLifeTimeRoot["endAlpha"] = overLifeTime.endAlpha;
		overLifeTimeRoot["isTransSpeed"] = overLifeTime.isTransSpeed;
		overLifeTimeRoot["startSpeed"] = overLifeTime.startSpeed;
		overLifeTimeRoot["endSpeed"] = overLifeTime.endSpeed;
		overLifeTimeRoot["Gravity"] = overLifeTime.gravity;
		overLifeTimeRoot["isRoring"] = overLifeTime.isRoring;
		Vector3 minRoringSpeed = overLifeTime.minRoringSpeed;
		overLifeTimeRoot["minRoringSpeed"] = json::array({ minRoringSpeed.x,minRoringSpeed.y ,minRoringSpeed.z });
		Vector3 maxRoringSpeed = overLifeTime.maxRoringSpeed;
		overLifeTimeRoot["maxRoringSpeed"] = json::array({ maxRoringSpeed.x,maxRoringSpeed.y ,maxRoringSpeed.z });
		overLifeTimeRoot["isNoise"] = overLifeTime.isNoise;
		overLifeTimeRoot["density"] = overLifeTime.density;
		overLifeTimeRoot["strength"] = overLifeTime.strength;

	}

	//rootに保存するパラメータの登録
	/*root[saveFileName_]["isLoop"] = particle_->particleData_.isLoop_;
	root[saveFileName_]["textureName"] = particle_->particleData_.textureName_;*/

	/*Vector3 emitTranslate = particle_->particleData_.emitter_.translate;
	root[saveFileName_]["Emitter"]["Translate"] = json::array({ emitTranslate.x, emitTranslate.y, emitTranslate.z });
	Vector3 emitSize = particle_->particleData_.emitter_.size;
	root[saveFileName_]["Emitter"]["Size"] = json::array({ emitSize.x, emitSize.y, emitSize.z });
	root[saveFileName_]["Emitter"]["Radius"] = particle_->particleData_.emitter_.radius;
	root[saveFileName_]["Emitter"]["Scale"] = particle_->particleData_.emitter_.scale;
	root[saveFileName_]["Emitter"]["Rotate"] = particle_->particleData_.emitter_.rotate;
	root[saveFileName_]["Emitter"]["Count"] = particle_->particleData_.emitter_.count;
	root[saveFileName_]["Emitter"]["Frequency"] = particle_->particleData_.emitter_.frequency;
	Vector4 emitColor = particle_->particleData_.emitter_.color;
	root[saveFileName_]["Emitter"]["Color"] = json::array({ emitColor.x, emitColor.y, emitColor.z, emitColor.w });
	root[saveFileName_]["Emitter"]["LifeTime"] = particle_->particleData_.emitter_.lifeTime;
	root[saveFileName_]["Emitter"]["Speed"] = particle_->particleData_.emitter_.speed;
	root[saveFileName_]["Emitter"]["EmitterType"] = particle_->particleData_.emitter_.emitterType;
	root[saveFileName_]["Emitter"]["BillboardType"] = particle_->particleData_.emitter_.billboardType;*/

	/*root[saveFileName_]["OverLifeTime"]["isConstantVelocity"] = particle_->particleData_.overLifeTime_.isConstantVelocity;
	Vector3 velocity = particle_->particleData_.overLifeTime_.velocity;
	root[saveFileName_]["OverLifeTime"]["Velocity"] = json::array({ velocity.x, velocity.y, velocity.z });
	root[saveFileName_]["OverLifeTime"]["isTransVelocity"] = particle_->particleData_.overLifeTime_.isTransVelocity;
	Vector3 startVelocity = particle_->particleData_.overLifeTime_.startVelocity;
	root[saveFileName_]["OverLifeTime"]["startVelocity"] = json::array({ startVelocity.x, startVelocity.y, startVelocity.z });
	Vector3 endVelocity = particle_->particleData_.overLifeTime_.endVelocity;
	root[saveFileName_]["OverLifeTime"]["endVelocity"] = json::array({endVelocity.x, endVelocity.y, endVelocity.z});
	root[saveFileName_]["OverLifeTime"]["isScale"] = particle_->particleData_.overLifeTime_.isScale;
	root[saveFileName_]["OverLifeTime"]["startScale"] = particle_->particleData_.overLifeTime_.startScale;
	root[saveFileName_]["OverLifeTime"]["endScale"] = particle_->particleData_.overLifeTime_.endScale;
	root[saveFileName_]["OverLifeTime"]["isColor"] = particle_->particleData_.overLifeTime_.isColor;
	Vector3 startColor = particle_->particleData_.overLifeTime_.startColor;
	root[saveFileName_]["OverLifeTime"]["startColor"] = json::array({ startColor.x, startColor.y, startColor.z });
	Vector3 endColor = particle_->particleData_.overLifeTime_.endColor;
	root[saveFileName_]["OverLifeTime"]["endColor"] = json::array({ endColor.x, endColor.y, endColor.z });
	root[saveFileName_]["OverLifeTime"]["isAlpha"] = particle_->particleData_.overLifeTime_.isAlpha;
	root[saveFileName_]["OverLifeTime"]["startAlpha"] = particle_->particleData_.overLifeTime_.startAlpha;
	root[saveFileName_]["OverLifeTime"]["midAlpha"] = particle_->particleData_.overLifeTime_.midAlpha;
	root[saveFileName_]["OverLifeTime"]["endAlpha"] = particle_->particleData_.overLifeTime_.endAlpha;
	root[saveFileName_]["OverLifeTime"]["isTransSpeed"] = particle_->particleData_.overLifeTime_.isTransSpeed;
	root[saveFileName_]["OverLifeTime"]["startSpeed"] = particle_->particleData_.overLifeTime_.startSpeed;
	root[saveFileName_]["OverLifeTime"]["endSpeed"] = particle_->particleData_.overLifeTime_.endSpeed;
	root[saveFileName_]["OverLifeTime"]["Gravity"] = particle_->particleData_.overLifeTime_.gravity;
	root[saveFileName_]["OverLifeTime"]["isRoring"] = particle_->particleData_.overLifeTime_.isRoring;
	Vector3 minRoringSpeed = particle_->particleData_.overLifeTime_.minRoringSpeed;
	root[saveFileName_]["OverLifeTime"]["minRoringSpeed"] = json::array({ minRoringSpeed.x,minRoringSpeed.y ,minRoringSpeed.z });
	Vector3 maxRoringSpeed = particle_->particleData_.overLifeTime_.maxRoringSpeed;
	root[saveFileName_]["OverLifeTime"]["maxRoringSpeed"] = json::array({ maxRoringSpeed.x,maxRoringSpeed.y ,maxRoringSpeed.z });
	root[saveFileName_]["OverLifeTime"]["isNoise"] = particle_->particleData_.overLifeTime_.isNoise;
	root[saveFileName_]["OverLifeTime"]["density"] = particle_->particleData_.overLifeTime_.density;
	root[saveFileName_]["OverLifeTime"]["strength"] = particle_->particleData_.overLifeTime_.strength;*/


	//ディレクトリがなければ作成する
	std::filesystem::path dir(kDirectoryPath_);
	if (!std::filesystem::exists(kDirectoryPath_)) {
		std::filesystem::create_directory(kDirectoryPath_);
	}
	//書き込むjsonファイルのフルパスを合成する
	std::string filePath = kDirectoryPath_ + saveFileName_ + ".json";
	//書き込む用のファイルストリーム
	std::ofstream ofs;
	//ファイルを開く
	ofs.open(filePath);

	//open失敗
	if (ofs.fail()) {
		std::string message = "Failed open data file.";
		MessageBoxA(nullptr, message.c_str(), "ParticleEditor", 0);
		assert(0);
		return;
	} else {
		//成功したら
		std::string message = "The file was saved successfully";
		MessageBoxA(nullptr, message.c_str(), "ParticleEditor", 0);
	}

	//ファイルにjsonの文字列を書き込む(インデント幅4)
	ofs << std::setw(4) << root << std::endl;
	//ファイルを閉じる
	ofs.close();

}

void ParticleEditor::LoadDataFile(const std::string& fileName) {


	//ファイル名が入力されていなかったら
	if (saveFileName_.empty()) {
		std::string message = "FileName is empty.";
		MessageBoxA(nullptr, message.c_str(), "ParticleEditor", 0);
		return;
	}

	//読み込むjsonファイルのフルパスを合成
	std::string filePath = kDirectoryPath_ + fileName + ".json";

	std::ifstream ifs;

	//ファイルがあるかどうか
	if (!std::filesystem::exists(filePath)) {
		std::string message = "File not found";
		MessageBoxA(nullptr, message.c_str(), "ParticleEditor", 0);
		return;
	}

	ifs.open(filePath);

	//open失敗
	if (ifs.fail()) {
		std::string message = "Failed open data file.";
		MessageBoxA(nullptr, message.c_str(), "ParticleEditor", 0);
		return;
	}

	json root;
	//json文字列からjsonのデータ構造に展開
	ifs >> root;
	//ファイルを閉じる
	ifs.close();

	//データがあるかのチェック
	if (!root.contains(fileName)) {
		std::string message = "The file contains no data.";
		MessageBoxA(nullptr, message.c_str(), "ParticleEditor", 0);
		return;
	}

	const auto& groups = root[fileName];
	particles_.clear();

	for (const auto& [group, particleRoot] : groups.items()) {
		particles_[group].reset(GPUParticle::Create(TextureManager::Load("circle.png"), 100000));
		auto& data = particles_[group]->particleData_;

		data.isLoop_ = particleRoot["isLoop"].get<bool>();
		data.textureName_ = particleRoot["textureName"].get<std::string>();
		particles_[group]->SetTextureHandle();

		auto& emitterData = data.emitter_;
		const json& emitterRoot = particleRoot["Emitter"];

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
		const json& overLifeTimeRoot = particleRoot["OverLifeTime"];

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
		if (overLifeTimeRoot.contains("isRoring")) {
			overLifeTimeData.isRoring = overLifeTimeRoot["isRoring"].get<uint32_t>();
			json minRoringSpeed = overLifeTimeRoot["minRoringSpeed"];
			overLifeTimeData.minRoringSpeed = Vector3(static_cast<float>(minRoringSpeed[0]), static_cast<float>(minRoringSpeed[1]), static_cast<float>(minRoringSpeed[2]));
			json maxRoringSpeed = overLifeTimeRoot["maxRoringSpeed"];
			overLifeTimeData.maxRoringSpeed = Vector3(static_cast<float>(maxRoringSpeed[0]), static_cast<float>(maxRoringSpeed[1]), static_cast<float>(maxRoringSpeed[2]));
		}
		if (overLifeTimeRoot.contains("isNoise")) {
			overLifeTimeData.isNoise = overLifeTimeRoot["isNoise"].get<uint32_t>();
			overLifeTimeData.density = overLifeTimeRoot["density"].get<float>();
			overLifeTimeData.strength = overLifeTimeRoot["strength"].get<float>();
		}

	}

	//particle_->Init(TextureManager::Load("circle.png"), 100000);

	////ファイルのデータを設定していく
	//particle_->particleData_.isLoop_ = root[fileName]["isLoop"].get<bool>();
	//particle_->particleData_.textureName_ = root[fileName]["textureName"].get<std::string>();
	//particle_->SetTextureHandle();

	//auto& emitterData = particle_->particleData_.emitter_;
	//json& emitterRoot = root[fileName]["Emitter"];

	//json emitTranslate = emitterRoot["Translate"];
	//emitterData.translate = Vector3(static_cast<float>(emitTranslate[0]), static_cast<float>(emitTranslate[1]), static_cast<float>(emitTranslate[2]));
	//json emitSize = emitterRoot["Size"];
	//emitterData.size = Vector3(static_cast<float>(emitSize[0]), static_cast<float>(emitSize[1]), static_cast<float>(emitSize[2]));
	//emitterData.radius = emitterRoot["Radius"].get<float>();
	//emitterData.scale = emitterRoot["Scale"].get<float>();
	//emitterData.rotate = emitterRoot["Rotate"].get<float>();
	//emitterData.count = emitterRoot["Count"].get<uint32_t>();
	//emitterData.frequency = emitterRoot["Frequency"].get<float>();
	//json emitColor = emitterRoot["Color"];
	//emitterData.color = Vector4(static_cast<float>(emitColor[0]), static_cast<float>(emitColor[1]), static_cast<float>(emitColor[2]), static_cast<float>(emitColor[3]));
	//emitterData.lifeTime = emitterRoot["LifeTime"].get<float>();
	//emitterData.speed = emitterRoot["Speed"].get<float>();
	//emitterData.emitterType = emitterRoot["EmitterType"].get<uint32_t>();
	//if (emitterRoot.contains("BillboardType")) {
	//	emitterData.billboardType = emitterRoot["BillboardType"].get<uint32_t>();
	//}

	//auto& overLifeTimeData = particle_->particleData_.overLifeTime_;
	//json& overLifeTimeRoot = root[fileName]["OverLifeTime"];

	//overLifeTimeData.isConstantVelocity = overLifeTimeRoot["isConstantVelocity"].get<uint32_t>();
	//json velocity = overLifeTimeRoot["Velocity"];
	//overLifeTimeData.velocity = Vector3(static_cast<float>(velocity[0]), static_cast<float>(velocity[1]), static_cast<float>(velocity[2]));
	//overLifeTimeData.isTransVelocity = overLifeTimeRoot["isTransVelocity"].get<uint32_t>();
	//json startVelo = overLifeTimeRoot["startVelocity"];
	//overLifeTimeData.startVelocity = Vector3(static_cast<float>(startVelo[0]), static_cast<float>(startVelo[1]), static_cast<float>(startVelo[2]));
	//json endVelo = overLifeTimeRoot["endVelocity"];
	//overLifeTimeData.endVelocity = Vector3(static_cast<float>(endVelo[0]), static_cast<float>(endVelo[1]), static_cast<float>(endVelo[2]));
	//overLifeTimeData.isScale = overLifeTimeRoot["isScale"].get<uint32_t>();
	//overLifeTimeData.startScale = overLifeTimeRoot["startScale"].get<float>();
	//overLifeTimeData.endScale = overLifeTimeRoot["endScale"].get<float>();
	//overLifeTimeData.isColor = overLifeTimeRoot["isColor"].get<uint32_t>();
	//json startColor = overLifeTimeRoot["startColor"];
	//overLifeTimeData.startColor = Vector3(static_cast<float>(startColor[0]), static_cast<float>(startColor[1]), static_cast<float>(startColor[2]));
	//json endColor = overLifeTimeRoot["endColor"];
	//overLifeTimeData.endColor = Vector3(static_cast<float>(endColor[0]), static_cast<float>(endColor[1]), static_cast<float>(endColor[2]));
	//overLifeTimeData.isAlpha = overLifeTimeRoot["isAlpha"].get<uint32_t>();
	//overLifeTimeData.startAlpha = overLifeTimeRoot["startAlpha"].get<float>();
	//overLifeTimeData.midAlpha = overLifeTimeRoot["midAlpha"].get<float>();
	//overLifeTimeData.endAlpha = overLifeTimeRoot["endAlpha"].get<float>();
	//overLifeTimeData.isTransSpeed = overLifeTimeRoot["isTransSpeed"].get<uint32_t>();
	//overLifeTimeData.startSpeed = overLifeTimeRoot["startSpeed"].get<float>();
	//overLifeTimeData.endSpeed = overLifeTimeRoot["endSpeed"].get<float>();
	//overLifeTimeData.gravity = overLifeTimeRoot["Gravity"].get<float>();
	//if (overLifeTimeRoot.contains("isRoring")) {
	//	overLifeTimeData.isRoring = overLifeTimeRoot["isRoring"].get<uint32_t>();
	//	json minRoringSpeed = overLifeTimeRoot["minRoringSpeed"];
	//	overLifeTimeData.minRoringSpeed = Vector3(static_cast<float>(minRoringSpeed[0]), static_cast<float>(minRoringSpeed[1]), static_cast<float>(minRoringSpeed[2]));
	//	json maxRoringSpeed = overLifeTimeRoot["maxRoringSpeed"];
	//	overLifeTimeData.maxRoringSpeed = Vector3(static_cast<float>(maxRoringSpeed[0]), static_cast<float>(maxRoringSpeed[1]), static_cast<float>(maxRoringSpeed[2]));
	//}
	//if (overLifeTimeRoot.contains("isNoise")) {
	//	overLifeTimeData.isNoise = overLifeTimeRoot["isNoise"].get<uint32_t>();
	//	overLifeTimeData.density = overLifeTimeRoot["density"].get<float>();
	//	overLifeTimeData.strength = overLifeTimeRoot["strength"].get<float>();
	//}



}

void ParticleEditor::DebugGUI() {
#ifdef _DEBUG
	ImGui::Begin("ParticleEditor");

	bool isCheck = false;
	char strBuff[256];

	strncpy_s(strBuff, saveFileName_.c_str(), sizeof(strBuff));
	strBuff[sizeof(strBuff) - 1] = '\0';

	if (ImGui::InputText("FileName", strBuff, sizeof(strBuff))) {
		saveFileName_ = strBuff;
	}

	if (ImGui::Button("LoadFile")) {
		LoadDataFile(saveFileName_);
	}

	ImGui::SameLine();

	if (ImGui::Button("Save")) {
		DataSave();
	}

	strncpy_s(strBuff, addParticelName_.c_str(), sizeof(strBuff));
	strBuff[sizeof(strBuff) - 1] = '\0';
	if (ImGui::InputText("AddParticleName", strBuff, sizeof(strBuff))) {
		addParticelName_ = strBuff;
	}
	if (ImGui::Button("Add")) {
		if (addParticelName_.empty() || particles_.find(addParticelName_) != particles_.end()) {
			std::string message = "The name is already in use.";
			MessageBoxA(nullptr, message.c_str(), "ParticleEditor", 0);
		}else {
			particles_[addParticelName_].reset(GPUParticle::Create(TextureManager::Load("circle.png"), 100000));
			particles_[addParticelName_]->particleData_.isLoop_ = isLoop_;
		}
	}

	if (ImGui::Checkbox("isLoop", &isLoop_)) {
		for (auto& [group, particle] : particles_) {
			particle->particleData_.isLoop_ = isLoop_;
		}
	}
	if (!isLoop_) {
		if (ImGui::Button("Emit")) {
			for (auto& [group, particle] : particles_) {
				particle->Emit();
			}
		}
	}

	for (auto it = particles_.begin(); it != particles_.end();) {
		const auto& group = it->first;
		auto& particle = it->second;

		if (ImGui::TreeNode(group.c_str())) {

			auto& data = particle->particleData_;
			auto& emitter = data.emitter_;

			ImGui::InputFloat("speed", &emitter.speed);
			ImGui::InputFloat("Scale", &emitter.scale);
			ImGui::InputFloat("rotate", &emitter.rotate);
			ImGui::InputFloat3("size", &emitter.size.x);
			ImGui::InputInt("count", reinterpret_cast<int*>(&emitter.count));
			ImGui::InputFloat("Frequecy", &emitter.frequency);
			ImGui::InputFloat("radius", &emitter.radius);
			ImGui::InputFloat("LifeTime", &emitter.lifeTime);
			ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&emitter.color));

			strncpy_s(strBuff, data.textureName_.c_str(), sizeof(strBuff));
			strBuff[sizeof(strBuff) - 1] = '\0';
			if (ImGui::InputText("TextureName", strBuff, sizeof(strBuff))) {
				data.textureName_ = strBuff;
			}
			if (ImGui::Button("Apply")) {
				particle->SetTextureHandle();
			}

			int32_t currentBillboardType = static_cast<int32_t>(emitter.billboardType);

			if (ImGui::RadioButton("Billborad", currentBillboardType == GPUParticle::BillboardType::Billboard)) {
				currentBillboardType = GPUParticle::BillboardType::Billboard;
			}
			if (ImGui::RadioButton("HorizontalBillboard", currentBillboardType == GPUParticle::BillboardType::Horizontalillboard)) {
				currentBillboardType = GPUParticle::BillboardType::Horizontalillboard;
			}
			if (ImGui::RadioButton("None", currentBillboardType == GPUParticle::BillboardType::None)) {
				currentBillboardType = GPUParticle::BillboardType::None;
			}

			emitter.billboardType = static_cast<GPUParticle::BillboardType>(currentBillboardType);


			int32_t currentEmitShape = static_cast<int32_t>(emitter.emitterType);

			if (ImGui::RadioButton("Sphere", currentEmitShape == GPUParticle::EmitShape::Sphere)) {
				currentEmitShape = GPUParticle::EmitShape::Sphere;
			}
			if (ImGui::RadioButton("Hemisphere", currentEmitShape == GPUParticle::EmitShape::Hemisphere)) {
				currentEmitShape = GPUParticle::EmitShape::Hemisphere;
			}
			if (ImGui::RadioButton("Box", currentEmitShape == GPUParticle::EmitShape::Box)) {
				currentEmitShape = GPUParticle::EmitShape::Box;
			}
			if (ImGui::RadioButton("Square", currentEmitShape == GPUParticle::EmitShape::Squere)) {
				currentEmitShape = GPUParticle::EmitShape::Squere;
			}
			if (ImGui::RadioButton("Circle", currentEmitShape == GPUParticle::EmitShape::Circle)) {
				currentEmitShape = GPUParticle::EmitShape::Circle;
			}

			// 選択された形状を反映
			emitter.emitterType = static_cast<GPUParticle::EmitShape>(currentEmitShape);

			auto& overLifeTime = data.overLifeTime_;

			isCheck = (overLifeTime.isConstantVelocity != 0);
			if (ImGui::Checkbox("isConstantVelocity", &isCheck)) {
				overLifeTime.isConstantVelocity = static_cast<uint32_t>(isCheck);
			}
			if (overLifeTime.isConstantVelocity) {
				if (ImGui::TreeNode("ConstantVelocity")) {
					ImGui::InputFloat3("velocity", &overLifeTime.velocity.x);
					ImGui::TreePop();
				}
			}

			isCheck = (overLifeTime.isTransVelocity != 0);
			if (ImGui::Checkbox("isTransVelocity", &isCheck)) {
				overLifeTime.isTransVelocity = static_cast<uint32_t>(isCheck);
			}
			if (overLifeTime.isTransVelocity) {
				if (ImGui::TreeNode("TransVelocity")) {
					ImGui::InputFloat3("startVelocity", &overLifeTime.startVelocity.x);
					ImGui::InputFloat3("endVelocity", &overLifeTime.endVelocity.x);
					ImGui::TreePop();
				}
			}

			isCheck = (overLifeTime.isScale != 0);
			if (ImGui::Checkbox("isScale", &isCheck)) {
				overLifeTime.isScale = static_cast<uint32_t>(isCheck);
			}
			if (overLifeTime.isScale) {
				if (ImGui::TreeNode("ScaleOverLifeTime")) {
					ImGui::InputFloat("startScale", &overLifeTime.startScale);
					ImGui::InputFloat("endScale", &overLifeTime.endScale);
					ImGui::TreePop();
				}
			}

			isCheck = (overLifeTime.isColor != 0);
			if (ImGui::Checkbox("isColor", &isCheck)) {
				overLifeTime.isColor = static_cast<uint32_t>(isCheck);
			}
			if (overLifeTime.isColor) {
				if (ImGui::TreeNode("ColorOverLifeTime")) {
					ImGui::ColorEdit3("startColor", &overLifeTime.startColor.x);
					ImGui::ColorEdit3("endColor", &overLifeTime.endColor.x);
					ImGui::TreePop();
				}
			}

			isCheck = (overLifeTime.isAlpha != 0);
			if (ImGui::Checkbox("isAlpha", &isCheck)) {
				overLifeTime.isAlpha = static_cast<uint32_t>(isCheck);
			}
			if (overLifeTime.isAlpha) {
				if (ImGui::TreeNode("AlphaOverLifeTime")) {
					ImGui::SliderFloat("startAlpha", &overLifeTime.startAlpha, 0.0f, 1.0f);
					ImGui::SliderFloat("midAlpha", &overLifeTime.midAlpha, 0.0f, 1.0f);
					ImGui::SliderFloat("endAlpha", &overLifeTime.endAlpha, 0.0f, 1.0f);
					ImGui::TreePop();
				}
			}

			isCheck = (overLifeTime.isTransSpeed != 0);
			if (ImGui::Checkbox("isTransSpeed", &isCheck)) {
				overLifeTime.isTransSpeed = static_cast<uint32_t>(isCheck);
			}
			if (overLifeTime.isTransSpeed) {
				if (ImGui::TreeNode("SpeedOverLifeTime")) {
					ImGui::InputFloat("startSpeed", &overLifeTime.startSpeed);
					ImGui::InputFloat("endSpeed", &overLifeTime.endSpeed);
					ImGui::TreePop();
				}
			}

			ImGui::InputFloat("gravity", &overLifeTime.gravity);

			isCheck = (overLifeTime.isRoring != 0);
			if (ImGui::Checkbox("isRoring", &isCheck)) {
				overLifeTime.isRoring = static_cast<uint32_t>(isCheck);
			}
			if (overLifeTime.isRoring) {
				if (ImGui::TreeNode("roringSpeed")) {
					ImGui::InputFloat3("minRoringSpeed", &overLifeTime.minRoringSpeed.x);
					ImGui::InputFloat3("maxRoringSpeed", &overLifeTime.maxRoringSpeed.x);
					ImGui::TreePop();
				}
			}

			isCheck = (overLifeTime.isNoise != 0);
			if (ImGui::Checkbox("isNoise", &isCheck)) {
				overLifeTime.isNoise = static_cast<uint32_t>(isCheck);
			}
			if (overLifeTime.isNoise) {
				if (ImGui::TreeNode("NoiseState")) {
					ImGui::InputFloat("density", &overLifeTime.density);
					ImGui::InputFloat("strength", &overLifeTime.strength);
					ImGui::TreePop();
				}
			}


			if (ImGui::Button("Delete")) {
				it = particles_.erase(it);
				ImGui::TreePop();
				continue;
			}

			ImGui::TreePop();
		}

		++it;
	}
	
	
	


	ImGui::End();
#endif // _DEBUG
}
