#include "ParticleEditor.h"

#include "ImGuiManager.h"
#include "TextureManager.h"
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

	billboardList_ = {
		"BillBoard",
		"HorizonBillboard",
		"None",
	};

	emitterShapeList_ = {
		"NonShape",
		"Sphere",
		"Hemisphere",
		"Box",
		"Square",
		"Circle",
	};

	//particle_.reset(GPUParticle::Create(TextureManager::Load("circle.png"), 100000));
	saveFileName_ = "testParticle";
	LoadDataFile(saveFileName_);

	//currentBillboardMode_ = billboardComboList_[particle_->particleData_.emitter_.billboardType];
	//currentEmitterShape_ = emitterShapesComboList_[particle_->particleData_.emitter_.emitterType];

}

void ParticleEditor::Update() {
	DebugGUI();

	for (auto& [group, particle] : particles_) {
		particle->Update();
	}
}

void ParticleEditor::Draw(const DaiEngine::Camera& camera) {
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
		particleRoot["isModel"] = data.isModel_;
		particleRoot["modelName"] = data.modelName_;

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
		overLifeTimeRoot["density"] = json::array({ overLifeTime.density.x, overLifeTime.density.y, overLifeTime.density.z });
		overLifeTimeRoot["strength"] = overLifeTime.strength;
		overLifeTimeRoot["isRandom"] = overLifeTime.isRandom;
	}

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

		billboardComboList_[group] = billboardList_;
		emitterShapeComboList_[group] = emitterShapeList_;

		particles_[group].reset(DaiEngine::GPUParticle::Create(DaiEngine::TextureManager::Load("circle.png"), 10000));
		auto& data = particles_[group]->particleData_;

		//ファイルのデータを設定していく
		data.isLoop_ = isLoop_ = particleRoot["isLoop"].get<bool>();
		data.textureName_ = particleRoot["textureName"].get<std::string>();
		particles_[group]->SetTextureHandle();
		if (particleRoot.contains("isModel")) {
			data.isModel_ = particleRoot["isModel"].get<bool>();
			if (data.isModel_) {
				data.modelName_ = particleRoot["modelName"].get<std::string>();
				particles_[group]->SetModel();
			}
		}

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

		currentBillboardMode_[group] = billboardComboList_[group][emitterData.billboardType];
		currentEmitterShape_[group] = emitterShapeComboList_[group][emitterData.emitterType];

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
			json density = overLifeTimeRoot["density"];
			overLifeTimeData.density = Vector3(static_cast<float>(density[0]), static_cast<float>(density[1]), static_cast<float>(density[2]));
			overLifeTimeData.strength = overLifeTimeRoot["strength"].get<float>();
			if (overLifeTimeRoot.contains("isRandom")) {
				overLifeTimeData.isRandom = overLifeTimeRoot["isRandom"].get<uint32_t>();
			}
		}
	}

	//particle_->Init(TextureManager::Load("circle.png"), 100000);

	



}

void ParticleEditor::DebugGUI() {
#ifdef USE_IMGUI
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
		}
		else {
			particles_[addParticelName_].reset(DaiEngine::GPUParticle::Create(DaiEngine::TextureManager::Load("circle.png"), 10000));
			particles_[addParticelName_]->particleData_.isLoop_ = isLoop_;
			billboardComboList_[addParticelName_] = billboardList_;
			currentBillboardMode_[addParticelName_] = billboardList_[0];
			emitterShapeComboList_[addParticelName_] = emitterShapeList_;
			currentEmitterShape_[addParticelName_] = emitterShapeList_[1];
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

			const auto& textures = DaiEngine::TextureManager::GetInstance()->GetTextures();

			if (ImGui::TreeNode("Texture")) {

				int32_t count = 2;
				int32_t index = 0;

				ImGui::BeginChild("Texture List", ImVec2(0, 200), true);
				for (const auto& texture : textures) {

					if (texture.name == "") { continue; }

					if (ImGui::ImageButton(texture.name.c_str(), (ImTextureID)texture.textureSrvHandleGPU_.ptr, ImVec2(64, 64))) {
						data.textureName_ = texture.name;
						particle->SetTextureHandle();
					}

					index++;
					if (index < count) {
						ImGui::SameLine();
					}
					else {
						index = 0; // 次の行に移る
					}

				}
				ImGui::EndChild();

				ImGui::TreePop();
			}
			


			strncpy_s(strBuff, data.modelName_.c_str(), sizeof(strBuff));
			strBuff[sizeof(strBuff) - 1] = '\0';
			ImGui::Checkbox("isModel", &data.isModel_);

			if (data.isModel_) {
				if (ImGui::InputText("ModelName", strBuff, sizeof(strBuff))) {
					data.modelName_ = strBuff;
				}
				if (ImGui::Button("Set")) {
					particle->SetModel();
				}
			}
			else {
				particle->ModelReset();
			}

			int32_t billboardType = static_cast<int32_t>(emitter.billboardType);

			if (ImGui::BeginCombo("BillboardType", currentBillboardMode_[group].c_str())) {
				for (int32_t index = 0; index < billboardComboList_[group].size(); index++) {
					const bool isSelected = (currentBillboardMode_[group] == billboardComboList_[group][index].c_str());
					if (ImGui::Selectable(billboardComboList_[group][index].c_str(), isSelected)) {
						currentBillboardMode_[group] = billboardComboList_[group][index];
						billboardType = index;
					}
				}
				ImGui::EndCombo();
			}

			emitter.billboardType = static_cast<DaiEngine::GPUParticle::BillboardType>(billboardType);


			int32_t emitterType = static_cast<int32_t>(emitter.emitterType);

			if (ImGui::BeginCombo("EmitterType", currentEmitterShape_[group].c_str())) {
				for (int32_t index = 0; index < emitterShapeComboList_[group].size(); index++) {
					const bool isSelected = (currentEmitterShape_[group] == emitterShapeComboList_[group][index].c_str());
					if (ImGui::Selectable(emitterShapeComboList_[group][index].c_str(), isSelected)) {
						currentEmitterShape_[group] = emitterShapeComboList_[group][index];
						emitterType = index;
					}
				}
				ImGui::EndCombo();
			}

			// 選択された形状を反映
			emitter.emitterType = static_cast<DaiEngine::GPUParticle::EmitShape>(emitterType);

			auto& overlifeTime = data.overLifeTime_;

			isCheck = (overlifeTime.isConstantVelocity != 0);
			if (ImGui::Checkbox("isConstantVelocity", &isCheck)) {
				overlifeTime.isConstantVelocity = static_cast<uint32_t>(isCheck);
			}
			if (overlifeTime.isConstantVelocity) {
				if (ImGui::TreeNode("ConstantVelocity")) {
					ImGui::InputFloat3("velocity", &overlifeTime.velocity.x);
					ImGui::TreePop();
				}
			}

			isCheck = (overlifeTime.isTransVelocity != 0);
			if (ImGui::Checkbox("isTransVelocity", &isCheck)) {
				overlifeTime.isTransVelocity = static_cast<uint32_t>(isCheck);
			}
			if (overlifeTime.isTransVelocity) {
				if (ImGui::TreeNode("TransVelocity")) {
					ImGui::InputFloat3("startVelocity", &overlifeTime.startVelocity.x);
					ImGui::InputFloat3("endVelocity", &overlifeTime.endVelocity.x);
					ImGui::TreePop();
				}
			}

			isCheck = (overlifeTime.isScale != 0);
			if (ImGui::Checkbox("isScale", &isCheck)) {
				overlifeTime.isScale = static_cast<uint32_t>(isCheck);
			}
			if (overlifeTime.isScale) {
				if (ImGui::TreeNode("ScaleOverLifeTime")) {
					ImGui::InputFloat("startScale", &overlifeTime.startScale);
					ImGui::InputFloat("endScale", &overlifeTime.endScale);
					ImGui::TreePop();
				}
			}

			isCheck = (overlifeTime.isColor != 0);
			if (ImGui::Checkbox("isColor", &isCheck)) {
				overlifeTime.isColor = static_cast<uint32_t>(isCheck);
			}
			if (overlifeTime.isColor) {
				if (ImGui::TreeNode("ColorOverLifeTime")) {
					ImGui::ColorEdit3("startColor", &overlifeTime.startColor.x);
					ImGui::ColorEdit3("endColor", &overlifeTime.endColor.x);
					ImGui::TreePop();
				}
			}

			isCheck = (overlifeTime.isAlpha != 0);
			if (ImGui::Checkbox("isAlpha", &isCheck)) {
				overlifeTime.isAlpha = static_cast<uint32_t>(isCheck);
			}
			if (overlifeTime.isAlpha) {
				if (ImGui::TreeNode("AlphaOverLifeTime")) {
					ImGui::SliderFloat("startAlpha", &overlifeTime.startAlpha, 0.0f, 1.0f);
					ImGui::SliderFloat("midAlpha", &overlifeTime.midAlpha, 0.0f, 1.0f);
					ImGui::SliderFloat("endAlpha", &overlifeTime.endAlpha, 0.0f, 1.0f);
					ImGui::TreePop();
				}
			}

			isCheck = (overlifeTime.isTransSpeed != 0);
			if (ImGui::Checkbox("isTransSpeed", &isCheck)) {
				overlifeTime.isTransSpeed = static_cast<uint32_t>(isCheck);
			}
			if (overlifeTime.isTransSpeed) {
				if (ImGui::TreeNode("SpeedOverLifeTime")) {
					ImGui::InputFloat("statrSpeed", &overlifeTime.startSpeed);
					ImGui::InputFloat("endSpeed", &overlifeTime.endSpeed);
					ImGui::TreePop();
				}
			}

			ImGui::InputFloat("gravity", &overlifeTime.gravity);

			isCheck = (overlifeTime.isRoring != 0);
			if (ImGui::Checkbox("isRoring", &isCheck)) {
				overlifeTime.isRoring = static_cast<uint32_t>(isCheck);
			}
			if (overlifeTime.isRoring) {
				if (ImGui::TreeNode("roringSpeed")) {
					ImGui::InputFloat3("minRoringSpeed", &overlifeTime.minRoringSpeed.x);
					ImGui::InputFloat3("maxRoringSpeed", &overlifeTime.maxRoringSpeed.x);
					ImGui::TreePop();
				}
			}

			isCheck = (overlifeTime.isNoise != 0);
			if (ImGui::Checkbox("isNoise", &isCheck)) {
				overlifeTime.isNoise = static_cast<uint32_t>(isCheck);
			}
			if (overlifeTime.isNoise) {
				if (ImGui::TreeNode("NoiseState")) {
					ImGui::InputFloat3("density", &overlifeTime.density.x);
					ImGui::InputFloat("strength", &overlifeTime.strength);
					isCheck = (overlifeTime.isRandom != 0);
					if (ImGui::Checkbox("isRandom", &isCheck)) {
						overlifeTime.isRandom = static_cast<uint32_t>(isCheck);
					}
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
