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

	blockManager_->SetViewProjection(view);
	LoadStageData();
	ApplyStageData();
}

void Stage::Reset()
{
	blockManager_->Reset();
	// ステージの情報を反映
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

void Stage::SaveStageData()
{
}

void Stage::ApplyStageData()
{
	StageArray<Element>& data = kStageData_.array_;
	BaseBlock::StageVector size = kStageData_.kMaxStageSize_;
	// 地面の位置を設定
	for (size_t i = 0; i < size.x; i++) {
		for (size_t j = 0; j < size.z; j++) {
			data[i][size.y - 1][j] = Element::kBlock;
		}
	}
	data[3][3][0] = Element::kBlock;

	data[3][3][3] = Element::kBody;
	data[1][3][2] = Element::kBody;
	// プレイヤーの位置を設定
	data[1][3][3] = Element::kPlayer;

	mapManager_->SetStageData(kStageData_);
	blockManager_->SetStageData(kStageData_);
}
