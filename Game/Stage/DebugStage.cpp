#include "DebugStage.h"

#include "../../ImGuiManager.h"

using Element = BaseBlock::Element;
template<typename Ty>
using StageArray = MapManager::StageArray<Ty>;

DebugStage::DebugStage()
{
}

DebugStage::~DebugStage()
{
}

void DebugStage::Initialize(ViewProjection* view)
{
	mapManager_ = MapManager::GetInstance();
	blockManager_ = BlockManager::GetInstance();
	variables_ = GlobalVariables::GetInstance();

	blockManager_->SetViewProjection(view);

	StageArray<Element>& data = kStageData_.array_;
	BaseBlock::StageVector size = kStageData_.kMaxStageSize_ = { 5,5,5 };
	//size = { 5,5,5 };
	data.resize(size.x);
	for (size_t i = 0; i < size.x; i++)
	{
		data[i].resize(size.y);
		for (size_t j = 0; j < size.y; j++)
		{
			data[i][j].resize(size.z);
			for (size_t k = 0; k < size.z; k++)
			{
				data[i][j][k] = Element::kNone;
			}
		}
	}
	//LoadStageData();
	//ApplyStageData();
}

void DebugStage::Reset()
{
	blockManager_->Reset();
	// ステージの情報を反映
	LoadStageData();
	ApplyStageData();
}

void DebugStage::Update()
{
	DebugGUI();

	if (Input::GetInstance()->TriggerKey(DIK_R))
	{
		Reset();
	}

	mapManager_->Update();
	blockManager_->Update();
}

void DebugStage::Draw()
{
	blockManager_->Draw();
}

void DebugStage::DebugGUI()
{
#ifdef _DEBUG

	ImGui::Begin("DebugStage");

	ImGui::Text("%d %d %d", kStageData_.kMaxStageSize_.x, kStageData_.kMaxStageSize_.y, kStageData_.kMaxStageSize_.z);

	if (ImGui::Button("Save"))
	{
		SaveStageData();
	}

	if (ImGui::Button("LoadStageSize"))
	{
		std::string groupName = "Stage";
		std::string append = "-" + std::to_string(stageNum_ + 1);
		Vector3 vecSize{};
		vecSize = variables_->GetVec3Value(groupName + append, "StageSize");
		kStageData_.kMaxStageSize_ = { (size_t)vecSize.x,(size_t)vecSize.y,(size_t)vecSize.z };

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
					data[i][j][k] = Element::kNone;
				}
			}
		}
		Reset();
	}

	ImGui::SameLine();

	if (ImGui::Button("LoadAll"))
	{
		LoadStageData();
		Reset();
	}

	ImGui::End();

#endif // _DEBUG
}

void DebugStage::LoadStageData()
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
	// Element
	// "0" : 0,
	// ~  "124" : 0

	// ここでファイルを読み込むので本来とは違う
	//kStageData_.kMaxStageSize_ = { 5,5,5 };
	//StageArray<Element>& data = kStageData_.array_;
	//BaseBlock::StageVector size = kStageData_.kMaxStageSize_;
	//data.resize(size.x);
	//for (size_t i = 0; i < data.size(); i++)
	//{
	//	data[i].resize(size.y);
	//	for (size_t j = 0; j < data[i].size(); j++)
	//	{
	//		data[i][j].resize(size.z);
	//		for (size_t k = 0; k < data[i][j].size(); k++)
	//		{
	//			data[i][j][k] = Element::kNone;
	//		}
	//	}
	//}
}

void DebugStage::SaveStageData()
{
	std::string groupName = "Stage";
	std::string append = "-" + std::to_string(stageNum_ + 1);

	variables_->CreateGroup(groupName + append);
	Vector3 vecSize{};

	vecSize.x = (float)kStageData_.kMaxStageSize_.x;
	vecSize.y = (float)kStageData_.kMaxStageSize_.y;
	vecSize.z = (float)kStageData_.kMaxStageSize_.z;
	variables_->AddItem(groupName + append, "StageSize", vecSize);
	variables_->SetValue(groupName + append, "StageSize", vecSize);


	int index = 0;

	StageArray<Element>& data = kStageData_.array_;
	BaseBlock::StageVector size = kStageData_.kMaxStageSize_;
	for (size_t i = 0; i < size.x; i++)
	{
		for (size_t j = 0; j < size.y; j++)
		{
			for (size_t k = 0; k < size.z; k++)
			{
				variables_->AddItem(groupName + append, std::to_string(index), (int32_t)data[i][j][k]);
				variables_->SetValue(groupName + append, std::to_string(index), (int32_t)data[i][j][k]);
				index++;
			}
		}
	}
}

void DebugStage::ApplyStageData()
{
	StageArray<Element>& data = kStageData_.array_;
	BaseBlock::StageVector size = kStageData_.kMaxStageSize_;
	// 地面の位置を設定
	for (size_t i = 0; i < size.x; i++)
	{
		for (size_t j = 0; j < size.z; j++)
		{
			data[i][size.y - 1][j] = Element::kBlock;
		}
	}
	mapManager_->SetStageData(kStageData_);
	blockManager_->SetStageData(kStageData_);
	if (stageNum_ < 3)
	{
		mapManager_->SetCanUseShot(false);
	}
	else
	{
		mapManager_->SetCanUseShot(true);
	}
}
