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

	blockManager_->SetViewProjection(view);
	LoadStageData();
	ApplyStageData();
}

void DebugStage::Reset()
{
	blockManager_->Reset();
	// ステージの情報を反映
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

	if (ImGui::Button("Load")) {
		Reset();
	}

	ImGui::End();

#endif // _DEBUG
}

void DebugStage::LoadStageData()
{
	// グローバルバリアブルズを使う



	// ここでファイルを読み込むので本来とは違う
	kStageData_.kMaxStageSize_ = { 5,5,5 };
	StageArray<Element>& data = kStageData_.array_;
	BaseBlock::StageVector size = kStageData_.kMaxStageSize_;
	data.resize(size.x);
	for (size_t i = 0; i < data.size(); i++) {
		data[i].resize(size.y);
		for (size_t j = 0; j < data[i].size(); j++) {
			data[i][j].resize(size.z);
			for (size_t k = 0; k < data[i][j].size(); k++) {
				data[i][j][k] = Element::kNone;
			}
		}
	}
}

void DebugStage::SaveStageData()
{
}

void DebugStage::ApplyStageData()
{
	StageArray<Element>& data = kStageData_.array_;
	BaseBlock::StageVector size = kStageData_.kMaxStageSize_;
	// 地面の位置を設定
	for (size_t i = 0; i < size.x; i++) {
		for (size_t j = 0; j < size.z; j++) {
			data[i][size.y - 1][j] = Element::kBlock;
		}
	}
	data[2][2][0] = Element::kBlock;
	data[2][2][4] = Element::kBlock;
	data[4][3][0] = Element::kBlock;

	data[1][3][4] = Element::kHead;

	data[3][3][4] = Element::kBody;
	data[0][3][0] = Element::kBody;
	// プレイヤーの位置を設定
	data[0][2][0] = Element::kPlayer;

	mapManager_->SetStageData(kStageData_);
	blockManager_->SetStageData(kStageData_);
}
