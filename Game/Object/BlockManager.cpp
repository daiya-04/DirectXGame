#include "BlockManager.h"

#include "BlockList.h"
#include "../../ImGuiManager.h"

using Element = BaseBlock::Element;

BlockManager* BlockManager::GetInstance()
{
	static BlockManager instance;
	return &instance;
}

void BlockManager::Initialize()
{
	uint32_t modelHandle = ModelManager::Load("Block");
	for (size_t i = 0; i < kMaxNormalBlockNum_; i++)
	{
		arrModelNormal_[i].reset(Object3d::Create(modelHandle));
	}
	modelHandle = ModelManager::Load("Head");
	for (size_t i = 0; i < kMaxPlayerBlockNum_; i++)
	{
		arrModelPlayer_[i].reset(Object3d::Create(modelHandle));
	}
	modelHandle = ModelManager::Load("Box");
	for (size_t i = 0; i < kMaxHeadBlockNum_; i++)
	{
		arrModelHead_[i].reset(Object3d::Create(modelHandle));
	}
	modelHandle = ModelManager::Load("Body");
	for (size_t i = 0; i < kMaxBodyBlockNum_; i++)
	{
		arrModelBody_[i].reset(Object3d::Create(modelHandle));
	}
	Reset();
}

void BlockManager::Reset()
{
	listBlock_.clear();
	iNormalModel_ = 0;
	iPlayerModel_ = 0;
	iBodyModel_ = 0;
	iHeadModel_ = 0;
	mapBlock_.clear();
	mapBlock_.resize(kMapSize.x);
	for (size_t i = 0; i < kMapSize.x; i++)
	{
		mapBlock_[i].resize(kMapSize.y);
		for (size_t j = 0; j < kMapSize.y; j++)
		{
			mapBlock_[i][j].resize(kMapSize.z);
			for (size_t k = 0; k < kMapSize.z; k++)
			{
				mapBlock_[i][j][k] = nullptr;
			}
		}
	}
}

void BlockManager::Update()
{
	std::list<pBlock>::iterator itr = listBlock_.begin();
	for (; itr != listBlock_.end(); ++itr)
	{
		itr->get()->Update();
	}
}

void BlockManager::Draw()
{
	std::list<pBlock>::iterator itr = listBlock_.begin();
	for (; itr != listBlock_.end(); ++itr)
	{
		itr->get()->Draw();
	}
}

void BlockManager::SetStageData(const MapManager::StageData& data)
{
	kMapSize = data.kMaxStageSize_;
	Reset();
	BaseBlock* block = nullptr;
	for (size_t i = 0; i < data.array_.size(); i++)
	{
		for (size_t j = 0; j < data.array_[i].size(); j++)
		{
			for (size_t k = 0; k < data.array_[i][j].size(); k++)
			{
				switch (data.array_[i][j][k])
				{
				case  Element::kNone:
					break;
				case  Element::kPlayer:
					block = new PlayerBlock;
					listBlock_.emplace_back(CreatePlayerBlock({ i,j,k }));
					break;
				case  Element::kBody:
					listBlock_.emplace_back(CreateBodyBlock({ i,j,k }));
					break;
				case  Element::kBlock:
					listBlock_.emplace_back(CreateNormalBlock({ i,j,k }));
					break;
				default:
					break;
				}
			}
		}
	}
}

void BlockManager::SetBlockPosition(const BaseBlock::StageVector& prePos, const BaseBlock::StageVector& pos)
{
	if (prePos.x == pos.x && prePos.y == pos.y && prePos.z == pos.z) {
		return;
	}
	mapBlock_[pos.x][pos.y][pos.z] = mapBlock_[prePos.x][prePos.y][prePos.z];
	mapBlock_[prePos.x][prePos.y][prePos.z] = nullptr;

	mapBlock_[pos.x][pos.y][pos.z]->MoveMapPosition(pos);
}

bool BlockManager::GetIsStaging()
{
	bool result = false;
	std::list<pBlock>::iterator itr = listBlock_.begin();
	for (; itr != listBlock_.end(); ++itr)
	{
		if (itr->get()->GetIsStaging()) {
			result = true;
			break;
		}
	}

	return result;
}

void BlockManager::FallFloatingBlock()
{
	if (GetIsStaging()) {
		return;
	}
	std::list<pBlock>::iterator itr = listBlock_.begin();
	for (; itr != listBlock_.end(); ++itr)
	{
		BaseBlock::StageVector pos = itr->get()->GetMapPosition();
		ChainFall(pos);
	}

}

bool BlockManager::ChainFall(const BaseBlock::StageVector& pos)
{
	// 空気なら落とす
	BaseBlock* block = mapBlock_[pos.x][pos.y][pos.z];
	if (block == nullptr) {
		return true;
	}
	// 落下するブロックだったら
	if (block->GetElement() == Element::kPlayer ||
		block->GetElement() == Element::kBody) {
		BaseBlock::StageVector down = pos;
		down.y += 1;
		// 落ちるかの判定
		// 下に落下ブロックが続いているなら再帰
		if (ChainFall(down)) {
			block->FallMapPosition(down);
			mapBlock_[down.x][down.y][down.z] = mapBlock_[pos.x][pos.y][pos.z];
			mapBlock_[pos.x][pos.y][pos.z] = nullptr;
			return true;
		}
	}
	// 落ちないブロックなので処理しない
	return false;
}


BaseBlock* BlockManager::CreateNormalBlock(const BaseBlock::StageVector& pos) {
	BaseBlock* block = new NormalBlock;
	block->Initialize(pos);
	block->SetViewProjection(vp_);
	block->SetModel(arrModelNormal_[iNormalModel_].get());
	iNormalModel_++;
	mapBlock_[pos.x][pos.y][pos.z] = block;
	return block;
}
BaseBlock* BlockManager::CreatePlayerBlock(const BaseBlock::StageVector& pos) {

	BaseBlock* block = new PlayerBlock;
	block->Initialize(pos);
	block->SetViewProjection(vp_);
	block->SetModel(arrModelPlayer_[iPlayerModel_].get());
	iNormalModel_++;
	mapBlock_[pos.x][pos.y][pos.z] = block;
	return block;
}

BaseBlock* BlockManager::CreateHeadBlock(const BaseBlock::StageVector& pos) {

	BaseBlock* block = new NormalBlock;
	block->Initialize(pos);
	block->SetViewProjection(vp_);
	block->SetModel(arrModelHead_[iHeadModel_].get());
	iHeadModel_++;
	mapBlock_[pos.x][pos.y][pos.z] = block;
	return block;
}
BaseBlock* BlockManager::CreateBodyBlock(const BaseBlock::StageVector& pos) {

	BaseBlock* block = new BodyBlock;
	block->Initialize(pos);
	block->SetViewProjection(vp_);
	block->SetModel(arrModelBody_[iBodyModel_].get());
	iBodyModel_++;
	mapBlock_[pos.x][pos.y][pos.z] = block;
	return block;
}