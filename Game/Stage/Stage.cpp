#include "Stage.h"

#include "../../ImGuiManager.h"

using Element = BaseBlock::Element;
template<typename Ty>
using StageArray = MapManager::StageArray<Ty>;

Stage::Stage()
{
}

Stage::~Stage()
{
}

void Stage::Initialize(ViewProjection* view)
{
	mapManager_ = MapManager::GetInstance();
	blockManager_ = BlockManager::GetInstance();
	variables_ = GlobalVariables::GetInstance();

	blockManager_->SetViewProjection(view);
	LoadStageData();
	//ApplyStageData();
}

void Stage::Reset()
{
	blockManager_->Reset();
	// ステージの情報を反映
	LoadStageData();
	ApplyStageData();
}

void Stage::Update()
{
	DebugGUI();

	if (Input::GetInstance()->TriggerKey(DIK_R))
	{
		Reset();
	}

	mapManager_->Update();
	blockManager_->Update();
}

void Stage::Draw()
{
	blockManager_->Draw();
}

void Stage::DebugGUI()
{
#ifdef _DEBUG

	ImGui::Begin("Stage");

	if (ImGui::Button("Load")) {
		Reset();
	}

	ImGui::End();

#endif // _DEBUG
}

void Stage::LoadStageData()
{
	// グローバルバリアブルズを使う
	std::string groupName = "Stage";
	std::string append = "-" + std::to_string(stageNum_ + 1);
	Vector3 vecSize{};
	vecSize = variables_->GetVec3Value(groupName + append, "StageSize");
	kStageData_.kMaxStageSize_ = { (size_t)vecSize.x,(size_t)vecSize.y,(size_t)vecSize.z };
	// StageSize
	// 5,5,5  :  7,7,7

	int index = 0;

	StageArray<Element>& data = kStageData_.array_;
	BaseBlock::StageVector size = kStageData_.kMaxStageSize_;
	data.resize(size.x);
	for (size_t i = 0; i < size.x; i++)
	{
		data[i].resize(size.y);
		for (size_t j = 0; j < size.y; j++)
		{
			data[i][j].resize(size.z);
			for (size_t k = 0; k < size.z; k++)
			{
				data[i][j][k] = (Element)variables_->GetIntValue(groupName + append, std::to_string(index));
				index++;
			}
		}
	}
}

void Stage::ApplyStageData()
{
	mapManager_->SetStageData(kStageData_);
	blockManager_->SetStageData(kStageData_);
}
