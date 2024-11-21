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

	billboardComboList_ = {
		"BillBoard",
		"HorizonBillboard",
		"None",
	};

	emitterShapesComboList_ = {
		"NonShape",
		"Sphere",
		"Hemisphere",
		"Box",
		"Square",
		"Circle",
	};

	particle_.reset(GPUParticle::Create(TextureManager::Load("circle.png"), 100000));
	saveFileName_ = "testParticle";
	LoadDataFile(saveFileName_);

	currentBillboardMode_ = billboardComboList_[particle_->particleData_.emitter_.billboardType];
	currentEmitterShape_ = emitterShapesComboList_[particle_->particleData_.emitter_.emitterType];

}

void ParticleEditor::Update() {
	DebugGUI();

	particle_->Update();
}

void ParticleEditor::Draw(const Camera& camera) {

	particle_->Draw(camera);

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

	//rootに保存するパラメータの登録
	root[saveFileName_]["isLoop"] = particle_->particleData_.isLoop_;
	root[saveFileName_]["textureName"] = particle_->particleData_.textureName_;

	Vector3 emitTranslate = particle_->particleData_.emitter_.translate;
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
	root[saveFileName_]["Emitter"]["BillboardType"] = particle_->particleData_.emitter_.billboardType;

	root[saveFileName_]["OverLifeTime"]["isConstantVelocity"] = particle_->particleData_.overLifeTime_.isConstantVelocity;
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
	root[saveFileName_]["OverLifeTime"]["strength"] = particle_->particleData_.overLifeTime_.strength;


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

	particle_->Init(TextureManager::Load("circle.png"), 100000);

	//ファイルのデータを設定していく
	particle_->particleData_.isLoop_ = root[fileName]["isLoop"].get<bool>();
	particle_->particleData_.textureName_ = root[fileName]["textureName"].get<std::string>();
	particle_->SetTextureHandle();

	auto& emitterData = particle_->particleData_.emitter_;
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

	auto& overLifeTimeData = particle_->particleData_.overLifeTime_;
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

	isCheck = (particle_->particleData_.isLoop_ != 0);
	if (ImGui::Checkbox("isLoop", &isCheck)) {
		particle_->particleData_.isLoop_ = isCheck;
	}
	if (!particle_->particleData_.isLoop_) {
		if (ImGui::Button("Emit")) {
			particle_->particleData_.emitter_.emit = 1;
		}
	}
	
	ImGui::InputFloat("speed", &particle_->particleData_.emitter_.speed);
	ImGui::InputFloat("Scale", &particle_->particleData_.emitter_.scale);
	ImGui::InputFloat("rotate", &particle_->particleData_.emitter_.rotate);
	ImGui::InputFloat3("size", &particle_->particleData_.emitter_.size.x);
	ImGui::InputInt("count", reinterpret_cast<int*>(&particle_->particleData_.emitter_.count));
	ImGui::InputFloat("Frequecy", &particle_->particleData_.emitter_.frequency);
	ImGui::InputFloat("radius", &particle_->particleData_.emitter_.radius);
	ImGui::InputFloat("LifeTime", &particle_->particleData_.emitter_.lifeTime);
	ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&particle_->particleData_.emitter_.color));

	strncpy_s(strBuff, particle_->particleData_.textureName_.c_str(), sizeof(strBuff));
	strBuff[sizeof(strBuff) - 1] = '\0';
	if (ImGui::InputText("TextureName", strBuff, sizeof(strBuff))) {
		particle_->particleData_.textureName_ = strBuff;
	}
	if (ImGui::Button("Apply")) {
		particle_->SetTextureHandle();
	}

	int32_t billboardType = static_cast<int32_t>(particle_->particleData_.emitter_.billboardType);

	if (ImGui::BeginCombo("BillboardType", currentBillboardMode_.c_str())) {
		for (int32_t index = 0; index < billboardComboList_.size(); index++) {
			const bool isSelected = (currentBillboardMode_ == billboardComboList_[index].c_str());
			if (ImGui::Selectable(billboardComboList_[index].c_str(), isSelected)) {
				currentBillboardMode_ = billboardComboList_[index];
				billboardType = index;
			}
		}
		ImGui::EndCombo();
	}

	particle_->particleData_.emitter_.billboardType = static_cast<GPUParticle::BillboardType>(billboardType);


	int32_t emitterType = static_cast<int32_t>(particle_->particleData_.emitter_.emitterType);

	if (ImGui::BeginCombo("EmitterType", currentEmitterShape_.c_str())) {
		for (int32_t index = 0; index < emitterShapesComboList_.size(); index++) {
			const bool isSelected = (currentEmitterShape_ == emitterShapesComboList_[index].c_str());
			if (ImGui::Selectable(emitterShapesComboList_[index].c_str(), isSelected)) {
				currentEmitterShape_ = emitterShapesComboList_[index];
				emitterType = index;
			}
		}
		ImGui::EndCombo();
	}

	// 選択された形状を反映
	particle_->particleData_.emitter_.emitterType = static_cast<GPUParticle::EmitShape>(emitterType);



	isCheck = (particle_->particleData_.overLifeTime_.isConstantVelocity != 0);
	if (ImGui::Checkbox("isConstantVelocity", &isCheck)) {
		particle_->particleData_.overLifeTime_.isConstantVelocity = static_cast<uint32_t>(isCheck);
	}
	if (particle_->particleData_.overLifeTime_.isConstantVelocity) {
		if (ImGui::TreeNode("ConstantVelocity")) {
			ImGui::InputFloat3("velocity", &particle_->particleData_.overLifeTime_.velocity.x);
			ImGui::TreePop();
		}
	}

	isCheck = (particle_->particleData_.overLifeTime_.isTransVelocity != 0);
	if (ImGui::Checkbox("isTransVelocity", &isCheck)) {
		particle_->particleData_.overLifeTime_.isTransVelocity = static_cast<uint32_t>(isCheck);
	}
	if (particle_->particleData_.overLifeTime_.isTransVelocity) {
		if (ImGui::TreeNode("TransVelocity")) {
			ImGui::InputFloat3("startVelocity", &particle_->particleData_.overLifeTime_.startVelocity.x);
			ImGui::InputFloat3("endVelocity", &particle_->particleData_.overLifeTime_.endVelocity.x);
			ImGui::TreePop();
		}
	}

	isCheck = (particle_->particleData_.overLifeTime_.isScale != 0);
	if (ImGui::Checkbox("isScale", &isCheck)) {
		particle_->particleData_.overLifeTime_.isScale = static_cast<uint32_t>(isCheck);
	}
	if (particle_->particleData_.overLifeTime_.isScale) {
		if (ImGui::TreeNode("ScaleOverLifeTime")) {
			ImGui::InputFloat("startScale", &particle_->particleData_.overLifeTime_.startScale);
			ImGui::InputFloat("endScale", &particle_->particleData_.overLifeTime_.endScale);
			ImGui::TreePop();
		}
	}

	isCheck = (particle_->particleData_.overLifeTime_.isColor != 0);
	if (ImGui::Checkbox("isColor", &isCheck)) {
		particle_->particleData_.overLifeTime_.isColor = static_cast<uint32_t>(isCheck);
	}
	if (particle_->particleData_.overLifeTime_.isColor) {
		if (ImGui::TreeNode("ColorOverLifeTime")) {
			ImGui::ColorEdit3("startColor", &particle_->particleData_.overLifeTime_.startColor.x);
			ImGui::ColorEdit3("endColor", &particle_->particleData_.overLifeTime_.endColor.x);
			ImGui::TreePop();
		}
	}

	isCheck = (particle_->particleData_.overLifeTime_.isAlpha != 0);
	if (ImGui::Checkbox("isAlpha", &isCheck)) {
		particle_->particleData_.overLifeTime_.isAlpha = static_cast<uint32_t>(isCheck);
	}
	if (particle_->particleData_.overLifeTime_.isAlpha) {
		if (ImGui::TreeNode("AlphaOverLifeTime")) {
			ImGui::SliderFloat("startAlpha", &particle_->particleData_.overLifeTime_.startAlpha, 0.0f, 1.0f);
			ImGui::SliderFloat("midAlpha", &particle_->particleData_.overLifeTime_.midAlpha, 0.0f, 1.0f);
			ImGui::SliderFloat("endAlpha", &particle_->particleData_.overLifeTime_.endAlpha, 0.0f, 1.0f);
			ImGui::TreePop();
		}
	}

	isCheck = (particle_->particleData_.overLifeTime_.isTransSpeed != 0);
	if (ImGui::Checkbox("isTransSpeed", &isCheck)) {
		particle_->particleData_.overLifeTime_.isTransSpeed = static_cast<uint32_t>(isCheck);
	}
	if (particle_->particleData_.overLifeTime_.isTransSpeed) {
		if (ImGui::TreeNode("SpeedOverLifeTime")) {
			ImGui::InputFloat("statrSpeed", &particle_->particleData_.overLifeTime_.startSpeed);
			ImGui::InputFloat("endSpeed", &particle_->particleData_.overLifeTime_.endSpeed);
			ImGui::TreePop();
		}
	}

	ImGui::InputFloat("gravity", &particle_->particleData_.overLifeTime_.gravity);

	isCheck = (particle_->particleData_.overLifeTime_.isRoring != 0);
	if (ImGui::Checkbox("isRoring", &isCheck)) {
		particle_->particleData_.overLifeTime_.isRoring = static_cast<uint32_t>(isCheck);
	}
	if (particle_->particleData_.overLifeTime_.isRoring) {
		if (ImGui::TreeNode("roringSpeed")) {
			ImGui::InputFloat3("minRoringSpeed", &particle_->particleData_.overLifeTime_.minRoringSpeed.x);
			ImGui::InputFloat3("maxRoringSpeed", &particle_->particleData_.overLifeTime_.maxRoringSpeed.x);
			ImGui::TreePop();
		}
	}

	isCheck = (particle_->particleData_.overLifeTime_.isNoise != 0);
	if (ImGui::Checkbox("isNoise", &isCheck)) {
		particle_->particleData_.overLifeTime_.isNoise = static_cast<uint32_t>(isCheck);
	}
	if (particle_->particleData_.overLifeTime_.isNoise) {
		if (ImGui::TreeNode("NoiseState")) {
			ImGui::InputFloat("density", &particle_->particleData_.overLifeTime_.density);
			ImGui::InputFloat("strength", &particle_->particleData_.overLifeTime_.strength);
			ImGui::TreePop();
		}
	}
	


	ImGui::End();
#endif // _DEBUG
}
