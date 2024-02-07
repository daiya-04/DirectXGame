#include "TitleScene.h"

#include"SceneManager.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include"Ease/Ease.h"
#include"SelectScene.h"


TitleScene::~TitleScene(){

}

void TitleScene::Init() {	
	camera_.Init();
	camera_.translation_ = { 0.0f,2.1f,-7.0f };
	camera_.rotation_ = { 0.33f,0.0f,0.0f };


	uint32_t backGroundTex = TextureManager::Load("white.png");
	uint32_t titleTex = TextureManager::Load("title.png");
	uint32_t pressTex = TextureManager::Load("PressAButton.png");

	background_.reset(new Sprite(backGroundTex, { 640.0f,360.0f }, { 1280.0f,720.0f }, 
		0.0f, { 0.5f,0.5f }, { 0.2f,0.2f,0.2f,1.0f }));

	background_->Initialize();

	titleTrnas_.Init();
	titleTrnas_.translation_ = { 640.0f,230.0f,0.0f };
	titleTrnas_.scale_ = { 576.0f,192.0f,0.0f };

	title_.reset(new Sprite(titleTex, { titleTrnas_.translation_.x,titleTrnas_.translation_.y }, { titleTrnas_.scale_.x,titleTrnas_.scale_.y },
		0.0f, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }));

	title_->Initialize();

	pressTrnas_.Init();
	pressTrnas_.translation_ = { 640.0f,500.0f,0.0f };
	pressTrnas_.scale_ = { 508.0f,72.0f,0.0f };

	press_.reset(new Sprite(pressTex, { pressTrnas_.translation_.x,pressTrnas_.translation_.y }, { 256.0f,64.0f }, 0.0f, { 0.5f,0.5f }, { 1.0f,1.0f,1.0f,1.0f }));

	press_->Initialize();

	i = 0.0f;

	alpha = 1.0f;

	input_ = Input::GetInstance();
	
	isNext_ = false;


	//model
	skyModel_ = ModelManager::Load("skyDome");

	skyDomeObj_.reset(Object3d::Create(skyModel_));
	skyDomeWT_.Init();
	skyDomeWT_.scale_ = { 100.0f,100.0f,100.0f };

	floorModel_ = ModelManager::Load("Firld");

	floorObj_.reset(Object3d::Create(floorModel_));
	floorWT_.Init();
	floorWT_.translation_ = { 0.0f,-1.0f,0.0f };

	rockModel_ = ModelManager::Load("Rock");

	rockObj_.reset(Object3d::Create(rockModel_));
	rockWT_.Init();
	rockWT_.translation_.z = 4.0f;
	rockWT_.translation_.y = -1.0f;
	rockWT_.rotation_.y = 1.57f;
	rockWT_.scale_ = { 1.5f,0.3f,1.5f };

	ChackFiles();

	for (int i = 0; i < coralNum_; i++) {
		coralModel_[i] = ModelManager::Load(("DeadTree_" + std::to_string(i + 1)).c_str());
	}

	if (chackOnlyNumber == 1) {
		LoadFiles();
	}
	else {
		for (int i = 0; i < coralNum_; i++) {
			obj_[i] = Object3d::Create(coralModel_[i]);
			coralObj_.push_back(std::unique_ptr<Object3d>(obj_[i]));
			coralWT_.Init();
			coralWT_.translation_ = { i * 4.0f,-1.23f,-3.0f };
			coralWT_.scale_ = { 1.0f,1.0f,1.0f };
			coralsWT_.push_back(coralWT_);
		}
	}

	
}

void TitleScene::Update() {
	DebugGUI();

	camera_.UpdateViewMatrix();

	title_->SetPosition({ titleTrnas_.translation_.x,titleTrnas_.translation_.y });
	title_->SetSize({ titleTrnas_.scale_.x,titleTrnas_.scale_.y });

	press_->SetPosition({ pressTrnas_.translation_.x,pressTrnas_.translation_.y });
	press_->SetSize({ pressTrnas_.scale_.x,pressTrnas_.scale_.y });


	if (i < 0.0f) {
		magnification *= -1;
	}
	else if (i > 1.0f) {
		magnification *= -1;
	}
	
	i += (addSpeed_ * magnification);
	alpha = Ease::Easing(Ease::EaseName::EaseOutSine, 0.0f, 1.0f, i);

	press_->SetColor({ 1.0f,1.0f,1.0f,alpha });

	pressTrnas_.UpdateMatrix();
	titleTrnas_.UpdateMatrix();

	floorWT_.UpdateMatrix();
	skyDomeWT_.UpdateMatrix();
	rockWT_.UpdateMatrix();

	for (int i = 0; i < coralsWT_.size(); i++){
		coralsWT_[i].UpdateMatrix();
	}
	if (isNext_){
		t += 0.02f;
	}
	
	camera_.translation_ = Ease::Easing(Ease::EaseName::EaseInBack, { 0.0f,2.1f,-7.0f }, endPos_, t);
	
	if ((input_->TriggerKey(DIK_RETURN) || input_->TriggerButton(XINPUT_GAMEPAD_A)) && !isNext_) {
		isNext_ = true;
		
		SceneManager::GetInstance()->ChangeScene(AbstractSceneFactory::SceneName::Select);
	}
	
}

void TitleScene::DrawBackGround() {
	
	
}

void TitleScene::DrawModel() {
	floorObj_->Draw(floorWT_, camera_);
	skyDomeObj_->Draw(skyDomeWT_, camera_);
	rockObj_->Draw(rockWT_, camera_);

	for (int i = 0; i < coralObj_.size(); i++){
		coralObj_[i]->Draw(coralsWT_[i], camera_);
	}
}

void TitleScene::DrawParticleModel() {

}

void TitleScene::DrawParticle() {
	
}

void TitleScene::DrawUI() {
	if (!isNext_){
		press_->Draw();
		title_->Draw();
	}
	
}

void TitleScene::DebugGUI() {
#ifdef _DEBUG
	ImGui::Begin("CameraMove");
	ImGui::DragFloat("t", &t, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat3("Endpos", &endPos_.x, 0.01f);
	ImGui::End();

	ImGui::Begin("TitleSprite");
	ImGui::DragFloat3("titletrans", &titleTrnas_.translation_.x, 0.1f);
	ImGui::DragFloat3("titlescale", &titleTrnas_.scale_.x, 0.1f);
	ImGui::DragFloat3("presstrans", &pressTrnas_.translation_.x, 0.1f);
	ImGui::DragFloat3("pressscale", &pressTrnas_.scale_.x, 0.1f);
	ImGui::DragFloat3("Camera : pos", &camera_.translation_.x, 0.01f);
	ImGui::DragFloat3("Camera : rotate", &camera_.rotation_.x, 0.01f);

	ImGui::DragFloat("i", &i, 0.01f);
	ImGui::End();

	ImGui::Begin("Corals");
	for (int i = 0; i < coralsWT_.size(); i++){
		ImGui::DragFloat3(("coralTrans " + std::to_string(i)).c_str(), &coralsWT_[i].translation_.x, 0.1f);
	}
	ImGui::End();

	ImGui::Begin("OBJ", nullptr, ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("make Obj")) {
			if (ImGui::TreeNode("make obj")) {
				ImGui::DragFloat3("Scale", &coralWT_.scale_.x, 0.01f);
				ImGui::DragFloat3("rotate", &coralWT_.rotation_.x, 0.01f);
				ImGui::DragFloat3("Trans", &coralWT_.translation_.x, 0.1f);

				if (ImGui::Button("add coral")) {
					AddCoral(coralWT_);
				}
				ImGui::TreePop();
			}


			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("obj List")) {
			if (ImGui::BeginMenu("coral List")) {
				for (int i = 0; i < coralObj_.size(); i++){
					if (ImGui::TreeNode((std::to_string(i + 1) + "Coral").c_str())) {
						ImGui::DragFloat3("Trans", &coralsWT_[i].translation_.x, 0.1f);
						ImGui::DragFloat3("rotate", &coralsWT_[i].rotation_.x, 0.01f);
						ImGui::DragFloat3("Scale", &coralsWT_[i].scale_.x, 0.01f);
						if (ImGui::Button("erase")){
							if (OperationConfirmation()){
								std::vector<std::unique_ptr<Object3d>>::iterator position = coralObj_.begin() + i;
								std::vector<WorldTransform>::iterator WTposition = coralsWT_.begin() + i;
								
								if (position != coralObj_.end() && WTposition != coralsWT_.end()) {
									coralObj_.erase(position);
									coralsWT_.erase(WTposition);
									
								}	
								
							}

						}
						ImGui::TreePop();
					}
				}
				ImGui::EndMenu();
			}


			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("file")) {
			if (ImGui::Button("save json")) {
				if (OperationConfirmation()){
					SaveFile();
				}
				
			}
			if (ImGui::Button("FileOverWrite")) {
				if (OperationConfirmation()) {
					FileOverWrite();
				}
			}

			if (ImGui::Button("LoadFiles")) {
				if (OperationConfirmation()) {
					LoadFiles();
				}
			}
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
	ImGui::End();


#endif // _DEBUG

}

bool TitleScene::OperationConfirmation(){
	int result = MessageBox(nullptr, L"Do you want to continue?", L"Confirmation", MB_YESNO | MB_ICONQUESTION);
	if (result == IDYES){
		return true;
	}
	else if (result == IDNO){
		return false;
	}
	else {
		return false;
	}
	
}

void TitleScene::SaveFile(){
	//保存
	json root;
	root = json::object();
	//root[kItemName_] = json::object();
	
	for (int i = 0; i < coralsWT_.size(); i++) {
		root[kItemName_][i][0] = json::array({ 
				coralsWT_[i].scale_.x,
				coralsWT_[i].scale_.y,
				coralsWT_[i].scale_.z
		});
		root[kItemName_][i][1] = json::array({
				coralsWT_[i].rotation_.x,
				coralsWT_[i].rotation_.y,
				coralsWT_[i].rotation_.z
		});
		root[kItemName_][i][2] = json::array({
				coralsWT_[i].translation_.x,
				coralsWT_[i].translation_.y,
				coralsWT_[i].translation_.z
		});
		std::filesystem::path dir(kDirectoryPath);
		if (!std::filesystem::exists(kDirectoryName)) {
			std::filesystem::create_directory(kDirectoryName);
		}
		// 書き込むjsonファイルのフルパスを合成する
		std::string filePath = kDirectoryPath + kItemName_ + ".json";
		// 書き込み用ファイルストリーム
		std::ofstream ofs;
		// ファイルを書き込みように開く
		ofs.open(filePath);
		//ファイルオープン失敗
		if (ofs.fail()) {
			std::string message = "Failed open data file for write.";
			MessageBoxA(nullptr, message.c_str(), "Object", 0);
			assert(0);
			break;
		}
		//ファイルにjson文字列を書き込む(インデント幅4)
		ofs << std::setw(4) << root << std::endl;
		//ファイルを閉じる
		ofs.close();
	}

	std::string message = "File save completed.";
	MessageBoxA(nullptr, message.c_str(), "Object", 0);
}

void TitleScene::FileOverWrite(){
	//読み込むjsonファイルのフルパスを合成する
	std::string filePath = kDirectoryPath + kItemName_ + ".json";
	//読み込み用のファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込み用に開く
	ifs.open(filePath);

	//上書き用に読み取り
	json root;
	ifs >> root;
	ifs.close();

	json overWrite;

	int i = 0;
	for (int i = 0; i < coralsWT_.size(); i++) {
		overWrite[kItemName_][i][0] = json::array({
				coralsWT_[i].scale_.x,
				coralsWT_[i].scale_.y,
				coralsWT_[i].scale_.z
			});
		overWrite[kItemName_][i][1] = json::array({
				coralsWT_[i].rotation_.x,
				coralsWT_[i].rotation_.y,
				coralsWT_[i].rotation_.z
			});
		overWrite[kItemName_][i][2] = json::array({
				coralsWT_[i].translation_.x,
				coralsWT_[i].translation_.y,
				coralsWT_[i].translation_.z
			});
	}

	root[kItemName_] = overWrite;

	// 書き込み用ファイルストリーム
	std::ofstream ofs;
	// ファイルを書き込みように開く
	ofs.open(filePath);
	//ファイルオープン失敗
	if (ofs.fail()) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "Element", 0);
		assert(0);
		return;
	}
	//ファイルにjson文字列を書き込む(インデント幅4)
	ofs << std::setw(4) << root << std::endl;
	//ファイルを閉じる
	ofs.close();

	std::string message = "File overwriting completed.";
	MessageBoxA(nullptr, message.c_str(), "Element", 0);
}

void TitleScene::ChackFiles(){
	if (!std::filesystem::exists(kDirectoryPath)) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "Box", 0);
		assert(0);
		return;
	}

	std::filesystem::directory_iterator dir_it(kDirectoryPath);

	for (const std::filesystem::directory_entry& entry : dir_it) {
		//ファイルパスを取得
		const std::filesystem::path& filePath = entry.path();

		//ファイル拡張子を取得
		std::string extension = filePath.extension().string();
		//.jsonファイル以外はスキップ
		if (extension.compare(".json") != 0) {
			continue;
		}

		if (LoadChackItem(kDirectoryPath, filePath.stem().string())) {
			chackOnlyNumber = 1;
		}

		if (fileName.size() != 0) {
			bool noneFail = true;
			for (size_t i = 0; i < fileName.size(); i++) {
				if (fileName[i].c_str() == filePath.stem().string()) {
					noneFail = false;
				}
			}
			if (noneFail) {
				fileName.push_back(filePath.stem().string());
			}

		}
		else {
			//ファイルの名前を取得
			fileName.push_back(filePath.stem().string());
		}
	}
}

void TitleScene::LoadFiles(){
	if (!LoadChackItem(kDirectoryPath, kItemName_))
		return;
	if (!std::filesystem::exists(kDirectoryPath)) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "Animetion", 0);
		assert(0);
		return;
	}

	std::filesystem::directory_iterator dir_it(kDirectoryPath);

	for (const std::filesystem::directory_entry& entry : dir_it) {
		//ファイルパスを取得
		const std::filesystem::path& filePath = entry.path();

		//ファイル拡張子を取得
		std::string extension = filePath.extension().string();
		//.jsonファイル以外はスキップ
		if (extension.compare(".json") != 0) {
			continue;
		}

		if (filePath.stem().string() == kItemName_) {
			//ファイル読み込み
			LoadFile(filePath.stem().string());
			return;
		}


	}
}

void TitleScene::LoadFile(const std::string& groupName){
	//読み込むjsonファイルのフルパスを合成する
	std::string filePath = kDirectoryPath + groupName + ".json";
	//読み込み用のファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込み用に開く
	ifs.open(filePath);
	// ファイルオープン失敗
	if (ifs.fail()) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "Adjustment_Item", 0);
		assert(0);
		return;
	}

	nlohmann::json root;

	//json文字列からjsonのデータ構造に展開
	ifs >> root;
	//ファイルを閉じる
	ifs.close();

	coralObj_.clear();

	coralsWT_.clear();

	//グループを検索
	nlohmann::json::iterator itGroup = root.find(groupName);
	//未登録チェック
	assert(itGroup != root.end());
	//各アイテムについて
	for (const auto& i : root[groupName]) {
		int count = 0;
		WorldTransform newTrans{};
		bool isNewMove = false;
		for (const auto& j : i) {
			Vector3 v{};

			if (count < 3) {
				from_json(j, v);
			}

			if (count == 0) {
				newTrans.scale_ = v;
			}
			else if (count == 1) {
				newTrans.rotation_ = v;
			}
			else if (count == 2) {
				newTrans.translation_ = v;
			}
			

			count++;

		}
		AddCoral(newTrans);
	}
#ifdef _DEBUG
	std::string message = "File loading completed";
	MessageBoxA(nullptr, message.c_str(), "Object", 0);

#endif // _DEBUG

}

bool TitleScene::LoadChackItem(const std::string& directoryPath, const std::string& itemName){
	// 書き込むjsonファイルのフルパスを合成する
	std::string filePath = kDirectoryPath + itemName + ".json";
	//読み込み用のファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込み用に開く
	ifs.open(filePath);
	// ファイルオープン失敗
	if (ifs.fail()) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "Adjustment_Item", 0);
		ifs.close();
		return false;
	}
	nlohmann::json root;

	//json文字列からjsonのデータ構造に展開
	ifs >> root;
	//ファイルを閉じる
	ifs.close();
	//グループを検索
	nlohmann::json::iterator itGroup = root.find(itemName);
	//未登録チェック
	if (itGroup != root.end()) {
		return true;
	}
	else {
		return false;
	}
}

void TitleScene::AddCoral(const WorldTransform worldTrans){
	WorldTransform newWorld = worldTrans;
	auto& coral = coralObj_.emplace_back(std::make_unique<Object3d>());
	coral->Initialize(coralModel_[coralsWT_.size() % 9]);

	newWorld.Init();
	coralsWT_.push_back(newWorld);
}

void TitleScene::from_json(const json& j, Vector3& v){
	v.x = j.at(0).get<float>();
	v.y = j.at(1).get<float>();
	v.z = j.at(2).get<float>();
}
