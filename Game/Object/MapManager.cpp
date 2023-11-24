#include "MapManager.h"

#include <cassert>

#include "./BlockManager.h"
#include "./BaseBlock.h"
#include "../../ImGuiManager.h"

using Element = BaseBlock::Element;

MapManager* MapManager::GetInstance()
{
	static MapManager instance;
	return &instance;
}

void MapManager::Initialize()
{
	input_ = Input::GetInstance();
	blockManager_ = BlockManager::GetInstance();

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

	// 状態を変える処理はここから



	// まずブロックが空中にあるかどうかを判定する


	// 空中から堕としている(演出している時は操作を受け付けても変更しない)
	// フラグとか？
	// 操作を受け取って状態を変える
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

	ImGui::Text("Player : %d,%d,%d", playerPosition_.x, playerPosition_.y, playerPosition_.z);

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
	preData_.array_ = currentData_.array_;
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

	// 演出中でなく
	// 動いていたら
	if (!isStaging_ && direct != dNONE) {
		MoveMainObject(direct);
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

void MapManager::MoveMainObject(MoveDirect direct)
{
	BaseBlock::StageVector temp = playerPosition_;
	if (CheckDirectPlayer(playerPosition_, direct, Element::kPlayer)) {
		// 動いたことをプレイヤーに伝える
		blockManager_->SetBlockPosition(temp, playerPosition_);

		// 動くことが出来たなら下のブロックも判定する
		do
		{
			// 頭の一個下
			temp.y += 1;
			// 下が体か確認する
			if (GetElement(temp) != Element::kBody) {
				break;
			}
			// 終了条件は体じゃない要素が出てくること
		} while (CheckDirectBody(temp, direct, Element::kBody, playerPosition_));

	}
}

bool MapManager::CheckDirectPlayer(BaseBlock::StageVector position, MoveDirect direct, BaseBlock::Element element)
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
		for (size_t i = z + 1; i < data[x][y].size(); i++)
		{
			// ブロックだった時
			if (data[x][y][i] == Element::kBlock) {
				SetElement(position, Element::kNone);
				position = { x,y,i - 1 };
				SetElement(position, element);
				playerPosition_ = position;
				return true;
			}
			// 体だった時
			if (data[x][y][i] == Element::kBody) {
				SetElement(position, Element::kNone);
				position = GetOnBody({ x,y,i });
				SetElement(position, element);
				playerPosition_ = position;
				return true;
			}
		}
		break;
	case MapManager::dBACK:
		// z 軸
		for (size_t i = z; 0 < i; i--)
		{
			// ブロックだった時
			if (data[x][y][i - 1] == Element::kBlock) {
				SetElement(position, Element::kNone);
				position = { x,y,i };
				SetElement(position, element);
				playerPosition_ = position;
				return true;
			}
			// 体だった時
			if (data[x][y][i - 1] == Element::kBody) {
				SetElement(position, Element::kNone);
				position = GetOnBody({ x,y,i - 1 });
				SetElement(position, element);
				playerPosition_ = position;
				return true;
			}
		}
		break;
	case MapManager::dRIGHT:
		// x 軸
		for (size_t i = x + 1; i < data.size(); i++)
		{
			// ブロックだった時
			if (data[i][y][z] == Element::kBlock) {
				SetElement(position, Element::kNone);
				position = { i - 1,y,z };
				SetElement(position, element);
				playerPosition_ = position;
				return true;
			}
			// 体だった時
			if (data[i][y][z] == Element::kBody) {
				SetElement(position, Element::kNone);
				position = GetOnBody({ i,y,z });
				SetElement(position, element);
				playerPosition_ = position;
				return true;
			}
		}
		break;
	case MapManager::dLEFT:
		// z 軸
		for (size_t i = x; 0 < i; i--)
		{
			// ブロックだった時
			if (data[i - 1][y][z] == Element::kBlock) {
				SetElement(position, Element::kNone);
				position = { i,y,z };
				SetElement(position, element);
				playerPosition_ = position;
				return true;
			}
			// 体だった時
			if (data[i - 1][y][z] == Element::kBlock) {
				SetElement(position, Element::kNone);
				position = GetOnBody({ i - 1,y,z });
				SetElement(position, element);
				playerPosition_ = position;
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

bool MapManager::CheckDirectBody(BaseBlock::StageVector position, MoveDirect direct, BaseBlock::Element element, BaseBlock::StageVector limit)
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
		for (size_t i = z + 1; i < limit.z + 1; i++)
		{
			// ブロックだった時
			if (data[x][y][i] == Element::kBlock || data[x][y][i] == Element::kBody) {
				SetElement(position, Element::kNone);
				position = { x,y,i - 1 };
				SetElement(position, element);
				return true;
			}
		}
		SetElement(position, Element::kNone);
		position = { x,y,limit.z };
		SetElement(position, element);
		return true;
		break;
	case MapManager::dBACK:
		// z 軸
		for (size_t i = z; limit.z < i; i--)
		{
			// ブロックだった時
			if (data[x][y][i - 1] == Element::kBlock || data[x][y][i - 1] == Element::kBody) {
				SetElement(position, Element::kNone);
				position = { x,y,i };
				SetElement(position, element);
				return true;
			}
		}
		SetElement(position, Element::kNone);
		position = { x,y,limit.z };
		SetElement(position, element);
		break;
	case MapManager::dRIGHT:
		// x 軸
		for (size_t i = x + 1; i < limit.x + 1; i++)
		{
			// ブロックだった時
			if (data[i][y][z] == Element::kBlock || data[i][y][z] == Element::kBody) {
				SetElement(position, Element::kNone);
				position = { i - 1,y,z };
				SetElement(position, element);
				return true;
			}
		}
		SetElement(position, Element::kNone);
		position = { limit.x,y,z };
		SetElement(position, element);
		break;
	case MapManager::dLEFT:
		// x 軸
		for (size_t i = x; limit.x < i; i--)
		{
			// ブロックだった時
			if (data[i - 1][y][z] == Element::kBlock || data[i - 1][y][z] == Element::kBody) {
				SetElement(position, Element::kNone);
				position = { i,y,z };
				SetElement(position, element);
				return true;
			}
		}
		SetElement(position, Element::kNone);
		position = { limit.x,y,z };
		SetElement(position, element);
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

const BaseBlock::Element& MapManager::GetElement(BaseBlock::StageVector position)
{
	return currentData_.array_[position.x][position.y][position.z];
}

BaseBlock::StageVector MapManager::GetOnBody(const BaseBlock::StageVector& pos)
{
	BaseBlock::StageVector position = pos;
	if (GetElement(position) == Element::kBody) {
		// 一段上
		position.y -= 1;
		position = GetOnBody(position);
	}
	return position;
}
