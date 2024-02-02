#pragma once
#include "IScene.h"
#include <memory>
#include <vector>
#include <list>
#include <random>
#include<fstream>

#include "Sprite.h"
#include "Object3d.h"
#include "Particle.h"
#include "Camera.h"
#include "WorldTransform.h"
#include <json.hpp>

class TitleScene : public IScene {
public:

	void Init()override;

	void Update()override;

	void DrawBackGround()override;

	void DrawModel()override;

	void DrawParticleModel()override;

	void DrawParticle()override;

	void DrawUI()override;

	void DebugGUI()override;

	~TitleScene()override;

private:

	std::unique_ptr<Sprite> background_;

	std::unique_ptr<Sprite> title_;

	std::unique_ptr<Sprite> press_;

	WorldTransform titleTrnas_;

	WorldTransform pressTrnas_;

	bool isNext_ = false;

	Input* input_ = nullptr;

	float i = 0.0f;

	float addSpeed_ = 0.04f;

	float magnification = 1.0f;

	float alpha = 1.0f;

	Camera camera_;

	//モデル
	static const int coralNum_ = 9;
	Model* skyModel_ = 0;
	Model* floorModel_ = 0;
	Model* coralModel_[coralNum_]{ 0 };


	std::unique_ptr<Object3d> skyDomeObj_;
	std::unique_ptr<Object3d> floorObj_;
	Object3d* obj_[coralNum_];
	std::vector<std::unique_ptr<Object3d>> coralObj_;

	WorldTransform skyDomeWT_;
	WorldTransform floorWT_;
	WorldTransform coralWT_;
	std::vector<WorldTransform> coralsWT_;

	//ファイル保存のあれこれ
public:
	using json = nlohmann::json;
	//その動作を行うかどうかの確認
	bool OperationConfirmation();

	void SaveFile();

	void FileOverWrite();

	void ChackFiles();

	void LoadFiles();

	void LoadFile(const std::string& groupName);

	bool LoadChackItem(const std::string& directoryPath, const std::string& itemName);

	void AddCoral(const WorldTransform worldTrans);

private:


	void from_json(const json& j, Vector3& v);

private:
	//ファイル保存関連
	int chackOnlyNumber = 0;

	const std::string kDirectoryPath = "Resources/Title_Object/";

	const std::string kDirectoryName = "Resources/Title_Object";

	const std::string kItemName_ = "Coral";

	std::string Name_ = "\0";

	char ItemName_[256]{};

	std::vector<std::string> fileName;
private:

	

	


};

 