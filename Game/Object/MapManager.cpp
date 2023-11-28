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
	BaseBlock::StageVector size = currentData_.kMaxStageSize_;
	data.reserve(size.x);
	// x 軸
	for (size_t i = 0; i < data.size(); i++)
	{
		data[i].reserve(size.y);
		// y 軸
		for (size_t j = 0; j < data[i].size(); j++)
		{
			data[i][j].reserve(size.z);
			// z 軸
			for (size_t k = 0; k < data[i][j].size(); k++)
			{
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
	FallFloatingBlock();

	// ブロックが演出中でない
	isStaging_ = blockManager_->GetIsStaging();

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

	ImGui::Text("(Player): %d,%d,%d", playerPosition_.x, playerPosition_.y, playerPosition_.z);
	ImGui::Text("Player  : %d,%d,%d", playerChunk_.position_.x, playerChunk_.position_.y - playerChunk_.bodyNum_, playerChunk_.position_.z);
	ImGui::Text("Position: %d,%d,%d", playerChunk_.position_.x, playerChunk_.position_.y, playerChunk_.position_.z);
	ImGui::Text("Bodys   : %d", playerChunk_.bodyNum_);


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
	if (ImGui::TreeNode("StageInfoAll"))
	{

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

void MapManager::SetStageData(const StageData& data)
{
	currentData_ = data;
	GetPlayerInfomation();
	preData_ = currentData_;
	moveLists_.clear();
	isShotFlag_ = false;
}

void MapManager::FallFloatingBlock()
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
				ChainFall({ i,j,k });
			}
		}
	}
	blockManager_->FallFloatingBlock();
}

bool MapManager::ChainFall(const BaseBlock::StageVector& pos)
{
	StageArray<Element>& data = currentData_.array_;
	// 空気なら落とす
	Element element = data[pos.x][pos.y][pos.z];
	if (element == Element::kNone)
	{
		return true;
	}
	// 落下するブロックだったら
	if (element == Element::kPlayer ||
		element == Element::kBody)
	{
		BaseBlock::StageVector down = pos;
		down.y += 1;
		// 落ちるかの判定
		// 下に落下ブロックが続いているなら再帰
		if (ChainFall(down))
		{
			//element->FallMapPosition(down);
			SetElement(pos, Element::kNone);
			SetElement(down, element);
			if (element == Element::kPlayer)
			{
				playerPosition_ = down;
			}
			return true;
		}
	}
	// 落ちないブロックなので処理しない
	return false;
}

void MapManager::GetOperate()
{
	MoveDirect direct = dNONE;
	if (input_->TriggerKey(DIK_B))
	{
		isShotFlag_ = !isShotFlag_;
	}

	if (input_->TriggerKey(DIK_W))
	{
		direct = dFRONT;
	}
	if (input_->TriggerKey(DIK_S))
	{
		direct = dBACK;
	}
	if (input_->TriggerKey(DIK_A))
	{
		direct = dLEFT;
	}
	if (input_->TriggerKey(DIK_D))
	{
		direct = dRIGHT;
	}

	// 演出中でなく
	// 動いていたら
	if (!isStaging_)
	{
		if (direct != dNONE)
		{
			if (isShotFlag_)
			{
				ShotSubObject(direct);
			}
			// プレイヤーが動くとき
			else
			{
				MoveMainObject(direct);
			}
		}
		GetPlayerChunk();
	}

}
void MapManager::GetPlayerInfomation()
{
	StageArray<Element>& data = currentData_.array_;
	bool isFind = false;
	// x 軸
	for (size_t i = 0; i < data.size(); i++)
	{
		// y 軸
		for (size_t j = 0; j < data[i].size(); j++)
		{
			// z 軸
			for (size_t k = 0; k < data[i][j].size(); k++)
			{
				if (data[i][j][k] == Element::kPlayer)
				{
					playerPosition_ = { i,j,k };
					isFind = true;
					break;
				}
			}
			if (isFind)
			{
				break;
			}
		}
		if (isFind)
		{
			break;
		}
	}
	GetPlayerChunk();
}

void MapManager::GetPlayerChunk()
{
	StageArray<Element>& data = currentData_.array_;
	// プレイヤーの下が体かどうか判別
	for (size_t y = playerPosition_.y; y < data[playerPosition_.x].size(); y++)
	{
		if (data[playerPosition_.x][y][playerPosition_.z] == Element::kBlock)
		{
			playerChunk_.position_ = playerPosition_;
			playerChunk_.position_.y = y - 1;
			playerChunk_.bodyNum_ = y - 1 - playerPosition_.y;
			break;
		}
	}

}

void MapManager::MoveMainObject(MoveDirect direct)
{
	//	BaseBlock::StageVector temp = playerPosition_;
	//	if (CheckDirectPlayer(playerPosition_, direct, Element::kPlayer)) {
	//		// 動いたことをプレイヤーに伝える
	//		blockManager_->SetBlockPosition(temp, playerPosition_);
	//
	//		// 頭の一個下
	//		temp.y += 1;
	//		BaseBlock::StageVector blockPos{};
	//		// 動くことが出来たなら下のブロックも判定する
	//		while (GetElement(temp) == Element::kBody) {
	//			// 終了条件は体じゃない要素が出てくること
	//			CheckDirectBody(temp, direct, Element::kBody, playerPosition_, blockPos);
	//			blockManager_->SetBlockPosition(temp, blockPos);
	//
	//			// 下が体か確認する
	//			// 頭の一個下
	//			temp.y += 1;
	//		}
	//	}

	// プレイヤーの頭をメインに考える
	CheckMoveAction(direct);

	InspecMovetAction(direct);

	ApplyMoveAction();


}

void MapManager::CheckMoveAction(MoveDirect direct)
{
	moveLists_.clear();

	// 下から動けるか確認する
	for (size_t i = 0; i < playerChunk_.bodyNum_ + 1; i++)
	{
		BaseBlock::StageVector position = playerChunk_.position_;
		position.y = position.y - i;
		moveLists_.push_back(CheckDirect(position, direct));
	}

}

void MapManager::InspecMovetAction(MoveDirect direct)
{
	// 実際に動いたときにどうなるか判定する
	StageArray<Element>& data = currentData_.array_;
	std::list<InspecterMove>::iterator itr = moveLists_.begin();

	// 頭が動かなかったときは判定しない
	if (moveLists_.back().result_ == MovedResult::kFAIL)
	{
		return;
	}
	// マップの端っこにいる時
	if (moveLists_.back().result_ == MovedResult::kOVER)
	{
		switch (direct)
		{
		case MapManager::dFRONT:
			if (moveLists_.back().end_.z == currentData_.kMaxStageSize_.z - 1)
			{
				return;
			}
			break;
		case MapManager::dBACK:
			if (moveLists_.back().end_.z == 0)
			{
				return;
			}
			break;
		case MapManager::dRIGHT:
			if (moveLists_.back().end_.x == currentData_.kMaxStageSize_.x - 1)
			{
				return;
			}
			break;
		case MapManager::dLEFT:
			if (moveLists_.back().end_.x == 0)
			{
				return;
			}
			break;
		case MapManager::dDOWN:
			break;
		case MapManager::dNONE:
			break;
		default:
			break;
		}
	}
	// ブロックの上に乗れるなら乗るようにする
	// 上に頭があるときは処理しない
	size_t x = moveLists_.begin()->end_.x;
	size_t y = moveLists_.begin()->end_.y;
	size_t z = moveLists_.begin()->end_.z;
	switch (direct)
	{
	case MapManager::dFRONT:
		// 一番下だけ検知
		// 体だった時
		if (data[x][y][z + 1] == Element::kBody && data[x][y - 1][z + 1] == Element::kNone)
		{
			for (; itr != moveLists_.end(); itr++)
			{
				itr->result_ = MovedResult::kSUCCECES;
				itr->end_ = { x,y - 1,z + 1 };
			}
		}
		break;
	case MapManager::dBACK:
		// 体の時
		if (data[x][y][z - 1] == Element::kBody && data[x][y - 1][z - 1] == Element::kNone)
		{
			for (; itr != moveLists_.end(); itr++)
			{
				itr->result_ = MovedResult::kSUCCECES;
				itr->end_ = { x,y - 1,z - 1 };
			}
		}
		break;
	case MapManager::dRIGHT:
		if (data[x + 1][y][z] == Element::kBody && data[x + 1][y - 1][z] == Element::kNone)
		{
			for (; itr != moveLists_.end(); itr++)
			{
				itr->result_ = MovedResult::kSUCCECES;
				itr->end_ = { x + 1,y - 1,z };
			}
		}
		break;
	case MapManager::dLEFT:
		if (data[x - 1][y][z] == Element::kBody && data[x - 1][y - 1][z] == Element::kNone)
		{
			for (; itr != moveLists_.end(); itr++)
			{
				itr->result_ = MovedResult::kSUCCECES;
				itr->end_ = { x - 1,y - 1,z };
			}
		}
		break;
	case MapManager::dDOWN:
		break;
	case MapManager::dNONE:
	default:
		break;
	}

	// 終わりの位置が同じ位置の時の処理
	itr = moveLists_.begin();
	for (; itr != moveLists_.end(); ++itr)
	{
		std::list<InspecterMove>::iterator itrB = itr;
		itrB++;
		for (; itrB != moveLists_.end(); ++itrB)
		{
			// 二個目の方が小さい時(上にあるはずのもの)
			if (itr->end_.y <= itrB->end_.y)
			{
				itrB->end_.y--;
			}
		}
	}

}

void MapManager::ApplyMoveAction()
{
	if (moveLists_.back().result_ == MovedResult::kFAIL)
	{
		return;
	}
	if (moveLists_.back().result_ == MovedResult::kOVER)
	{
		// ゲームオーバー演出
		return;
	}
	std::list<InspecterMove>::iterator itr = moveLists_.begin();

	playerPosition_ = moveLists_.back().end_;
	SetElement(moveLists_.back().end_, GetElement(moveLists_.back().start_));
	SetElement(moveLists_.back().start_, Element::kNone);
	blockManager_->SetBlockPosition(moveLists_.back().start_, moveLists_.back().end_);

	std::list<InspecterMove>::iterator end = moveLists_.begin();
	for (size_t i = 0; i < moveLists_.size() - 1; i++)
	{
		end++;
	}

	BaseBlock::StageVector blockPosition = playerPosition_;

	for (; itr != end; itr++)
	{
		if (itr->result_ == MovedResult::kOVER)
		{
			blockPosition.y = itr->start_.y;
			SetElement(blockPosition, GetElement(itr->start_));
			SetElement(itr->start_, Element::kNone);
			blockManager_->SetBlockPosition(itr->start_, blockPosition);
		}
		else if (itr->result_ == MovedResult::kSUCCECES)
		{
			SetElement(itr->end_, GetElement(itr->start_));
			SetElement(itr->start_, Element::kNone);
			blockManager_->SetBlockPosition(itr->start_, itr->end_);
		}
	}

}

void MapManager::ShotSubObject(MoveDirect direct)
{
	CheckShotAction(direct);

	InspectShotAction(direct);

	ApplyShotAction();
}

void MapManager::CheckShotAction(MoveDirect direct)
{
	moveLists_.clear();

	BaseBlock::StageVector position = playerChunk_.position_;
	// 一番下から射出判定
	moveLists_.push_back(CheckDirect(position, direct));


}

void MapManager::InspectShotAction(MoveDirect direct)
{
	// 実際に動いたときにどうなるか判定する
	StageArray<Element>& data = currentData_.array_;
	std::list<InspecterMove>::iterator itr = moveLists_.begin();

	// そもそも動かなかったときは判定しない
	if (itr->result_ == MovedResult::kFAIL)
	{
		return;
	}
	// マップの端っこにいる時
	if (itr->result_ == MovedResult::kOVER)
	{
		switch (direct)
		{
		case MapManager::dFRONT:
			if (itr->end_.z == currentData_.kMaxStageSize_.z - 1)
			{
				return;
			}
			break;
		case MapManager::dBACK:
			if (itr->end_.z == 0)
			{
				return;
			}
			break;
		case MapManager::dRIGHT:
			if (itr->end_.x == currentData_.kMaxStageSize_.x - 1)
			{
				return;
			}
			break;
		case MapManager::dLEFT:
			if (itr->end_.x == 0)
			{
				return;
			}
			break;
		case MapManager::dDOWN:
			break;
		case MapManager::dNONE:
			break;
		default:
			break;
		}
	}
	// ブロックの上に乗れるなら乗るようにする
	// 上に頭があるときは処理しない
	size_t x = itr->end_.x;
	size_t y = itr->end_.y;
	size_t z = itr->end_.z;
	switch (direct)
	{
	case MapManager::dFRONT:
		// 一番下だけ検知
		// 何かにぶつかった時
		if (data[x][y][z + 1] != Element::kNone)
		{
			itr->result_ = MovedResult::kSUCCECES;
			// 頭の時
			if (data[x][y][z + 1] == Element::kHead)
			{
				itr->end_ = { x,y,z + 1 };
			}
			else
			{
				itr->end_ = { x,y,z };
			}
		}
		break;
	case MapManager::dBACK:
		if (data[x][y][z - 1] != Element::kNone)
		{
			itr->result_ = MovedResult::kSUCCECES;
			// 頭の時
			if (data[x][y][z - 1] == Element::kHead)
			{
				itr->end_ = { x,y,z - 1 };
			}
			else
			{
				itr->end_ = { x,y,z };
			}
		}
		break;
	case MapManager::dRIGHT:
		if (data[x + 1][y][z] != Element::kNone)
		{
			itr->result_ = MovedResult::kSUCCECES;
			if (data[x + 1][y][z] == Element::kHead)
			{
				itr->end_ = { x + 1,y,z };
			}
			else
			{
				itr->end_ = { x,y,z };
			}
		}
		break;
	case MapManager::dLEFT:
		if (data[x - 1][y][z] != Element::kNone)
		{
			itr->result_ = MovedResult::kSUCCECES;
			if (data[x - 1][y][z] == Element::kHead)
			{
				itr->end_ = { x - 1,y,z };
			}
			else
			{
				itr->end_ = { x,y,z };
			}
		}
		break;
	case MapManager::dDOWN:
		break;
	case MapManager::dNONE:
	default:
		break;
	}

	//// 終わりの位置が同じ位置の時の処理
	//itr = moveLists_.begin();
	//for (; itr != moveLists_.end(); ++itr)
	//{
	//	std::list<InspecterMove>::iterator itrB = itr;
	//	itrB++;
	//	for (; itrB != moveLists_.end(); ++itrB)
	//	{
	//		// 二個目の方が小さい時(上にあるはずのもの)
	//		if (itr->end_.y <= itrB->end_.y)
	//		{
	//			itrB->end_.y--;
	//		}
	//	}
	//}
}

void MapManager::ApplyShotAction()
{
	std::list<InspecterMove>::iterator itr = moveLists_.begin();
	if (itr->result_ == MovedResult::kFAIL)
	{
		return;
	}
	if (itr->result_ == MovedResult::kOVER)
	{
		// ゲームオーバー演出
		return;
	}
	if (GetElement(itr->end_) == Element::kHead)
	{
		BaseBlock::StageVector headPos = { itr->end_.x,itr->end_.y - 1,itr->end_.z };
		SetElement(headPos, GetElement(itr->end_));
		blockManager_->SetBlockPosition(itr->end_, headPos);
		SetElement(itr->end_, GetElement(itr->start_));
		SetElement(itr->start_, Element::kNone);
		blockManager_->SetBlockPosition(itr->start_, itr->end_);
	}
	else
	{
		SetElement(itr->end_, GetElement(itr->start_));
		SetElement(itr->start_, Element::kNone);
		blockManager_->SetBlockPosition(itr->start_, itr->end_);
	}
}

MapManager::InspecterMove MapManager::CheckDirect(BaseBlock::StageVector position, MoveDirect direct)
{
	size_t& x = position.x;
	size_t& y = position.y;
	size_t& z = position.z;

	InspecterMove result;
	result.result_ = MovedResult::kFAIL;
	result.start_ = position;
	result.end_ = position;

	StageArray<Element>& data = currentData_.array_;
	// ここでカメラの方向によって動く方向を変えたい
	switch (direct)
	{
	case MapManager::dFRONT:
		// z 軸
		for (size_t i = z + 1; i < data[x][y].size(); i++)
		{
			// 空気以外の時
			if (data[x][y][i] != Element::kNone)
			{
				position = { x,y,i - 1 };
				result.result_ = MovedResult::kSUCCECES;
				result.end_ = position;
				return result;
			}
		}
		// 止まれなかったとき
		result.result_ = MovedResult::kOVER;
		break;
	case MapManager::dBACK:
		// z 軸
		for (size_t i = z; 0 < i; i--)
		{
			// 空気以外の時
			if (data[x][y][i - 1] != Element::kNone)
			{
				position = { x,y,i };
				result.result_ = MovedResult::kSUCCECES;
				result.end_ = position;
				return result;
			}
		}
		// 止まれなかったとき
		result.result_ = MovedResult::kOVER;
		break;
	case MapManager::dRIGHT:
		// x 軸
		for (size_t i = x + 1; i < data.size(); i++)
		{
			// 空気以外の時
			if (data[i][y][z] != Element::kNone)
			{
				position = { i - 1,y,z };
				result.result_ = MovedResult::kSUCCECES;
				result.end_ = position;
				return result;
			}
		}
		// 止まれなかったとき
		result.result_ = MovedResult::kOVER;
		break;
	case MapManager::dLEFT:
		// z 軸
		for (size_t i = x; 0 < i; i--)
		{
			// 空気以外の時
			if (data[i - 1][y][z] != Element::kNone)
			{
				position = { i,y,z };
				result.result_ = MovedResult::kSUCCECES;
				result.end_ = position;
				return result;
			}
		}
		// 止まれなかったとき
		result.result_ = MovedResult::kOVER;
		break;
	case MapManager::dDOWN:
		break;
	case MapManager::dNONE:
	default:
		break;
	}

	return result;
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
			if (data[x][y][i] == Element::kBlock)
			{
				SetElement(position, Element::kNone);
				position = { x,y,i - 1 };
				SetElement(position, element);
				playerPosition_ = position;
				return true;
			}
			// 体だった時
			if (data[x][y][i] == Element::kBody)
			{
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
			if (data[x][y][i - 1] == Element::kBlock)
			{
				SetElement(position, Element::kNone);
				position = { x,y,i };
				SetElement(position, element);
				playerPosition_ = position;
				return true;
			}
			// 体だった時
			if (data[x][y][i - 1] == Element::kBody)
			{
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
			if (data[i][y][z] == Element::kBlock)
			{
				SetElement(position, Element::kNone);
				position = { i - 1,y,z };
				SetElement(position, element);
				playerPosition_ = position;
				return true;
			}
			// 体だった時
			if (data[i][y][z] == Element::kBody)
			{
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
			if (data[i - 1][y][z] == Element::kBlock)
			{
				SetElement(position, Element::kNone);
				position = { i,y,z };
				SetElement(position, element);
				playerPosition_ = position;
				return true;
			}
			// 体だった時
			if (data[i - 1][y][z] == Element::kBlock)
			{
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

bool MapManager::CheckDirectBody(BaseBlock::StageVector position, MoveDirect direct, BaseBlock::Element element, BaseBlock::StageVector limit, BaseBlock::StageVector& nextPos)
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
			if (data[x][y][i] == Element::kBlock || data[x][y][i] == Element::kBody)
			{
				SetElement(position, Element::kNone);
				position = { x,y,i - 1 };
				nextPos = position;
				SetElement(position, element);
				return true;
			}
		}
		SetElement(position, Element::kNone);
		position = { x,y,limit.z };
		nextPos = position;
		SetElement(position, element);
		return true;
		break;
	case MapManager::dBACK:
		// z 軸
		for (size_t i = z; limit.z < i; i--)
		{
			// ブロックだった時
			if (data[x][y][i - 1] == Element::kBlock || data[x][y][i - 1] == Element::kBody)
			{
				SetElement(position, Element::kNone);
				position = { x,y,i };
				nextPos = position;
				SetElement(position, element);
				return true;
			}
		}
		SetElement(position, Element::kNone);
		position = { x,y,limit.z };
		nextPos = position;
		SetElement(position, element);
		break;
	case MapManager::dRIGHT:
		// x 軸
		for (size_t i = x + 1; i < limit.x + 1; i++)
		{
			// ブロックだった時
			if (data[i][y][z] == Element::kBlock || data[i][y][z] == Element::kBody)
			{
				SetElement(position, Element::kNone);
				position = { i - 1,y,z };
				nextPos = position;
				SetElement(position, element);
				return true;
			}
		}
		SetElement(position, Element::kNone);
		position = { limit.x,y,z };
		nextPos = position;
		SetElement(position, element);
		break;
	case MapManager::dLEFT:
		// x 軸
		for (size_t i = x; limit.x < i; i--)
		{
			// ブロックだった時
			if (data[i - 1][y][z] == Element::kBlock || data[i - 1][y][z] == Element::kBody)
			{
				SetElement(position, Element::kNone);
				position = { i,y,z };
				nextPos = position;
				SetElement(position, element);
				return true;
			}
		}
		SetElement(position, Element::kNone);
		position = { limit.x,y,z };
		nextPos = position;
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
	if (GetElement(position) == Element::kBody)
	{
		// 一段上
		position.y -= 1;
		position = GetOnBody(position);
	}
	return position;
}

size_t MapManager::GetMoveLength(const BaseBlock::StageVector& a, const BaseBlock::StageVector& b)
{
	size_t result = 0;

	if (a.x != b.x)
	{
		result = a.x < b.x ? b.x - a.x : a.x - b.x;
	}
	else if (a.z != b.z)
	{
		result = a.z < b.z ? b.z - a.z : a.z - b.z;
	}

	return result;
}
