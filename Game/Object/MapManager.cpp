#include "MapManager.h"

#include <cassert>

#include "./BaseBlock.h"
#include "../../ImGuiManager.h"

using Element = BaseBlock::Element;

void MapManager::Initialize()
{
	input_ = Input::GetInstance();

	StageArray<Element>& data = currentData_.array_;
	BaseBlock::StageVector position{ 0,0,0 };
	data.reserve(5);
	// x 軸
	for (size_t i = 0; i < data.size(); i++)
	{
		position.x = i;
		data[i].reserve(5);
		// y 軸
		for (size_t j = 0; j < data[i].size(); j++)
		{
			position.y = j;
			data[i][j].reserve(5);
			// z 軸
			for (size_t k = 0; k < data[i][j].size(); k++)
			{
				position.z = k;
				//block->SetViewProjection()
				data[i][j][k] = Element::kNone;
			}
		}
	}
}

void MapManager::Update()
{
	// 前の状態を保持
	preData_.array_ = currentData_.array_;

	GetOperate();

	DebugGUI();
}

void MapManager::Draw()
{
}

void MapManager::DebugGUI()
{
#ifdef _DEBUG
	StageArray<Element>& data = currentData_.array_;

	ImGui::Begin("MapManager");

	ImGui::Text("Player : %d,%d,%d", playerPosition_.x,playerPosition_.y,playerPosition_.z);

	std::string line[5];

	// 今のステージの情報を取る
	for (size_t i = 0; i < data.size(); i++)
	{
		line[i] = std::to_string(i) + "\n";
		line[i] += "x:z 0 1 2 3 4 5 6\n";
		for (size_t j = 0; j < data[i].size(); j++)
		{
			line[i] += std::to_string(j) + ": ";
			for (size_t k = 0; k < data[i][j].size(); k++)
			{
				line[i] += " " + std::to_string((size_t)data[j][i][k]);
			}
			line[i] += "\n";
		}
	}

	ImGui::Separator();
	if (ImGui::TreeNode("StageInfoAll")) {

		for (size_t i = 0; i < data.size(); i++)
		{
			ImGui::Text(line[i].c_str());
		}

		ImGui::TreePop();
	}
	ImGui::Separator();

	ImGui::End();

#endif // _DEBUG
}

void MapManager::SetStageData(const StageArray<BaseBlock::Element>& data)
{
	currentData_.array_ = data;
	GetPlayerPosition();
}

void MapManager::GetOperate()
{
	MoveDirect direct = dNONE;

	if (input_->TriggerKey(DIK_W)) {
		direct = dFRONT;
	}
	if (input_->TriggerKey(DIK_S)) {
		direct = dBACK;
	}
	if (input_->TriggerKey(DIK_A)) {
		direct = dLEFT;
	}
	if (input_->TriggerKey(DIK_D)) {
		direct = dRIGHT;
	}

	// 動いていたら
	if (direct != dNONE) {
		MoveStage(direct);
	}

}

void MapManager::GetPlayerPosition()
{
	StageArray<Element>& data = currentData_.array_;
	// x 軸
	for (size_t i = 0; i < data.size(); i++)
	{
		// y 軸
		for (size_t j = 0; j < data[i].size(); j++)
		{
			// z 軸
			for (size_t k = 0; k < data[i][j].size(); k++)
			{
				if (data[i][j][k] == Element::kPlayer) {
					playerPosition_ = { i,j,k };
					return;
				}
			}
		}
	}
}

void MapManager::MoveStage(MoveDirect direct)
{
	if (!CheckDirect(playerPosition_, direct)) {
		// 範囲外に出る
	}
}

bool MapManager::CheckDirect(BaseBlock::StageVector position, MoveDirect direct)
{
	size_t& x = position.x;
	size_t& y = position.y;
	size_t& z = position.z;

	StageArray<Element>& data = currentData_.array_;
	// ここでカメラの方向によって動く方向を変えたい
	switch (direct)
	{
	case MapManager::dFRONT:
		// z 軸
		for (size_t i = z; i < data[x][y].size(); i++)
		{
			// ブロックだった時
			if (data[x][y][i] == Element::kBlock) {
				SetElement(playerPosition_, Element::kNone);
				playerPosition_ = { x,y,i - 1 };
				SetElement(playerPosition_, Element::kPlayer);
				return true;
			}
		}
		break;
	case MapManager::dBACK:
		// z 軸
		for (size_t i = z + 1; 0 < i; i--)
		{
			// ブロックだった時
			if (data[x][y][i - 1] == Element::kBlock) {
				SetElement(playerPosition_, Element::kNone);
				playerPosition_ = { x,y,i };
				SetElement(playerPosition_, Element::kPlayer);
				return true;
			}
		}
		break;
	case MapManager::dRIGHT:
		// x 軸
		for (size_t i = x; i < data.size(); i++)
		{
			// ブロックだった時
			if (data[i][y][z] == Element::kBlock) {
				SetElement(playerPosition_, Element::kNone);
				playerPosition_ = { i - 1,y,z };
				SetElement(playerPosition_, Element::kPlayer);
				return true;
			}
		}
		break;
	case MapManager::dLEFT:
		// z 軸
		for (size_t i = x + 1; 0 < i; i--)
		{
			// ブロックだった時
			if (data[i - 1][y][z] == Element::kBlock) {
				SetElement(playerPosition_, Element::kNone);
				playerPosition_ = { i,y,z };
				SetElement(playerPosition_, Element::kPlayer);
				return true;
			}
		}
		break;
	case MapManager::dDOWN:
		break;
	case MapManager::dNONE:
	default:
		break;
	}

	return false;
}

void MapManager::SetElement(BaseBlock::StageVector position, BaseBlock::Element element)
{
	currentData_.array_[position.x][position.y][position.z] = element;
}
